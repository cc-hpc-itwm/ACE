/** \file
 *  \author Daniel Gruenewald, Leo Nesemann
 *  \date 2011-11-30, rewrite 2013-03
 */
#include "scheduler_Threading.h"
#include "scheduler_Table.h"

#include "utils/Sysinfo.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>


namespace scheduler
{

int Thread_environment::allTimeInstances_ = 0;


Thread_environment::Thread_environment(unsigned int nr_threads)    // = 0
  : nr_threads_(0)
  , pin_threads_(true)
{
  /// \todo make this thread-safe (should not be a Singleton, as an explicit constructor call is needed!)
  if( (++allTimeInstances_) != 1 )
  {
    throw std::runtime_error("scheduler::Thread_environment: Only one instance is allowed during the entire program execution");
  }

  Sysinfo system_info;

  std::cout << "================================================\n"
            << "Threading information:" << std::endl;

  {
    const int nr_available_threads = system_info.getPhyThreadCount() / system_info.getPhySocketCount();

    if ( nr_threads > nr_available_threads )
    {
      std::stringstream ss;
      ss << "Invalid number of threads to use argument: " << nr_threads << "; "
         << "Valid argument range = [0," << nr_available_threads << "]" << std::endl;
      //throw std::runtime_error(ss.str());
      std::cerr << ss.str();
      /// \todo Maybe we do need to throw a runtime error here?
    }

    nr_threads_ = ( (nr_threads == 0) ? nr_available_threads : nr_threads );

    std::cout << " -> starting " << nr_threads_ << " worker threads:" << std::endl;
  }

  {   // create the master thread pool and register the main thread:
    if (master_thread_pool_.mctpCreatePool(1) < 0)
    {
      throw std::runtime_error("Master thread pool creation failed");
    }
    master_thread_pool_.mctpRegisterThread();
  }

  {   // create the worker thread pool and set all workers to IDLE:
    if (worker_thread_pool_.mctpCreatePool(nr_threads_) < 0)
    {
      throw std::runtime_error("Worker thread pool creation failed");
    }

    worker_thread_parameters_.reserve(nr_threads_);
    for (int thread_id(0); thread_id < nr_threads_; ++thread_id)
    {
      worker_thread_parameters_.push_back(Thread( thread_id
    		                                    , pin_threads_
                                                , Thread::IDLE
                                                , NULL
                                                , &worker_thread_pool_
                                                , &master_thread_pool_) );
      worker_thread_pool_.mctpStartThread( workerThreadFunc
                                         , reinterpret_cast<void *> (&(worker_thread_parameters_[thread_id])) );
    }

    master_thread_pool_.mctpSyncPoolWith(&worker_thread_pool_);
  }

  std::cout << "================================================" << std::endl;
}


Thread_environment::~Thread_environment()
{
  for (int thread_id(0); thread_id < nr_threads_; ++thread_id)
  {
    if (worker_thread_parameters_[thread_id].state_ != Thread::IDLE)
    {
      std::stringstream ss;
      ss << "Worker thread " << thread_id << " is not IDLE, but has state "
         << worker_thread_parameters_[thread_id].state_ << "!\n";
      throw std::runtime_error(ss.str());
    }
    worker_thread_parameters_[thread_id].state_ = Thread::STOP_THREAD;
  }

  int nr_stopped(0);
  while ( nr_stopped != nr_threads_ )
  {
    nr_stopped = 0;
    for (int thread_id(0); thread_id < nr_threads_; ++thread_id)
    {
      if (worker_thread_parameters_[thread_id].state_ == Thread::STOPPED)
      {
        ++nr_stopped;
      }
    }

    mctp2Sleep(sleepTime_master_thread_);
  }

  std::cout << "All worker threads stopped." << std::endl;

  master_thread_pool_.mctpCleanup();
  worker_thread_pool_.mctpCleanup();
}


void Thread_environment::execute_schedule(Schedule& schedule)
{
  setSchedule(schedule);
  beginComputation();
  endComputation();
}


void Thread_environment::beginComputation()
{
  for(int thread_id(0); thread_id < nr_threads_; ++thread_id)
  {
    worker_thread_parameters_[thread_id].state_ = Thread::INIT_WORK;
  }
}


void Thread_environment::endComputation()
{
  int nr_working_threads(nr_threads_);

  while ( nr_working_threads != 0 )
  {
    nr_working_threads = nr_threads_;
    for(int thread_id(0); thread_id < nr_threads_; ++thread_id)
    {
      if( worker_thread_parameters_[thread_id].state_ == Thread::IDLE )
      {
        --nr_working_threads;
      }
      else if( worker_thread_parameters_[thread_id].state_ == Thread::EXCEPTION )
      {
    	  std::stringstream except;
    	  except<<"Thread "<<thread_id<<": "<<worker_thread_parameters_[thread_id].exceptinfo_;

    	  throw std::runtime_error(except.str());
      }
    }

    mctp2Sleep(sleepTime_master_thread_);
  }
  // at this point, all threads are IDLE.
}


void Thread_environment::setSchedule(Schedule& schedule)
{
  for (int thread_id(0); thread_id < nr_threads_; ++thread_id)
  {
    worker_thread_parameters_[thread_id].schedule_ = &schedule;
  }
}


void* Thread_environment::workerThreadFunc(void* ptr)
    // is static
{
  Thread* thread_parameters( reinterpret_cast<Thread*>(ptr) );
  mctp2& mctp_pool(*(thread_parameters->thread_pool_));

  mctp_pool.mctpRegisterThread();
  {
    Lock lock(Lock::Logging());
    std::cout << "    scheduling thread " << thread_parameters->thread_ID_;
    if ( thread_parameters->pin_thread_ )
    {
    	unsigned int usedCore( thread_parameters->pin() );
    	std::cout<< ": pinned to core " <<usedCore<<" !"<< std::endl;
    }
    else
    {
    	std::cout<< "." << std::endl;
    }
  }
  // synchronise with the "master" pool:
  mctp_pool.mctpSyncThreads();
  if (0 == thread_parameters->thread_ID_)
  {
    mctp_pool.mctpSyncPoolWith(thread_parameters->master_thread_pool_);
  }

  while( thread_parameters->state_ != Thread::STOP_THREAD )
  {
    switch (thread_parameters->state_)
    {
      case Thread::INIT_WORK:
        {
          thread_parameters->process_schedule();
          thread_parameters->schedule_ = NULL;
        }
        break;

      default:
        break;
    }

    mctp2Sleep(sleepTime_worker_thread_);
  }

  thread_parameters->state_ = Thread::STOPPED;

  return NULL;    // note: we need the (void*) (*f) (void*) signature for MCTP
}

std::vector<double>
Thread_environment::acquire_timings() const
{

  size_t const nr_generics_timers( worker_thread_parameters_[0].generics_timers_.size() );
  size_t const nr_specific_timers( worker_thread_parameters_[0].schedule_timers_.size() );
  size_t const nr_threads( worker_thread_parameters_.size() );


  for (size_t thread_id(1); thread_id < nr_threads; ++thread_id)
  {
	  if (worker_thread_parameters_[thread_id].generics_timers_.size() != nr_generics_timers)
	  {
		  throw std::logic_error("different numbers of generics timers for some threads in the ""worker"" pool!");
	  }
  }

  for (size_t thread_id(1); thread_id < nr_threads; ++thread_id)
  {
    if (worker_thread_parameters_[thread_id].schedule_timers_.size() != nr_specific_timers)
    {
      throw std::logic_error("different numbers of specific timers for some threads in the ""worker"" pool!");
    }
  }

  std::vector<double> timings;

  /// acquire generic timers total / spinlock
  for (size_t timer_id(0); timer_id < nr_generics_timers; ++timer_id)
  {
	  for (size_t thread_id(0); thread_id < nr_threads; ++thread_id)
	  {
		  timings.push_back( worker_thread_parameters_[thread_id].generics_timers_[timer_id].all_time() );
	  }
  }

  /// acquire schedule specific timers
  for (size_t timer_id(0); timer_id < nr_specific_timers; ++timer_id)
  {
	  for (size_t thread_id(0); thread_id < nr_threads; ++thread_id)
	  {
		  timings.push_back( worker_thread_parameters_[thread_id].schedule_timers_[timer_id].all_time() );
	  }
  }

  return timings;

}

void Thread_environment::print_timers() const
{
  size_t const nr_timers( worker_thread_parameters_[0].schedule_timers_.size() );
  for (size_t thread_id(1); thread_id < worker_thread_parameters_.size(); ++thread_id)
  {
    if (worker_thread_parameters_[thread_id].schedule_timers_.size() != nr_timers)
    {
      throw std::logic_error("different numbers of timers for some threads in the ""worker"" pool!");
    }
  }
  if (0 == nr_timers)
  {
    std::cout << "sorry, can not print any timers (there was no time measurement; you can add some with "
              << "Thread_environment::init_timers().)\n";
  }
  else
  {
	  std::cout<<table::generate_header()<<std::endl;

	  double tot_avg (0.);

	  for (size_t timer_id(0); timer_id < Timers::nr_timers(); ++timer_id)
	  {
		  std::vector<double> timings;

		  for (size_t thread_id(0); thread_id < worker_thread_parameters_.size(); ++thread_id)
		  {
			  timings.push_back( worker_thread_parameters_[thread_id].generics_timers_[timer_id].all_time() );
		  }

		  if( timer_id == Timers::TOTAL )
		  {
			  for( int i(0);i<timings.size();++i)
			  {
				  tot_avg += timings[i] ;
			  }
			  tot_avg /= static_cast<double>( timings.size() );
		  }

		  std::cout<<table::generate_column( worker_thread_parameters_[0].generics_timers_[timer_id].description()
				                           , timings
				                           , tot_avg )
		  <<std::endl;
	  }

	  std::cout<<table::generate_line()<<std::endl;

	  for (size_t timer_id(0); timer_id < nr_timers; ++timer_id)
	  {
		  std::vector<double> timings;
		  for (size_t thread_id(0); thread_id < worker_thread_parameters_.size(); ++thread_id)
		  {
			  timings.push_back( worker_thread_parameters_[thread_id].schedule_timers_[timer_id].all_time() );
		  }

		  std::cout<<table::generate_column( worker_thread_parameters_[0].schedule_timers_[timer_id].description()
				                           , timings
				                           , tot_avg )
		           <<std::endl;
	  }

	  std::cout<<table::generate_line()<<std::endl;
	  std::cout<<table::generate_footer()<<std::endl;
  }
}

void
Thread_environment::print_timers( OneSidedComm & comm ) const
{
	size_t const nr_generics_timers( worker_thread_parameters_[0].generics_timers_.size() );
	size_t const nr_specific_timers( worker_thread_parameters_[0].schedule_timers_.size() );

	size_t const nr_timers ( nr_generics_timers + nr_specific_timers );
	size_t const nr_threads( worker_thread_parameters_.size() );
	size_t const nr_ranks  ( comm.getNRanks() );


	if( comm.getRank() == 0 )
	{
		std::vector<std::vector<double> > global_timings( comm.getNRanks() );

		global_timings[0] = acquire_timings();

		for( int i(1);i<nr_ranks;++i)
		{
			std::vector<char> SrcData;
			int               SrcRank;
			comm.recvMessg(SrcData,SrcRank);
			for( int iVal(0);iVal<SrcData.size()/sizeof(double);++iVal)
			{
				global_timings[SrcRank].push_back( *(reinterpret_cast<double*>(&SrcData.front()) + iVal) );
			}
			if(global_timings[SrcRank].size() != global_timings[0].size() )
			{
				throw std::runtime_error("Sizes of global timings vector differ");
			}
		}

		std::cout<<table::generate_header()<<std::endl;

		double              global_tot_avg ( 0.                   );
		std::vector<double> local_tot_avg  ( nr_ranks , 0. );

		for( int i_rank(0); i_rank<nr_ranks;++i_rank)
		{
			for( int i_thread(0);i_thread<nr_threads;++i_thread)
			{
				double val( global_timings[i_rank][Timers::TOTAL * nr_threads + i_thread] );

				local_tot_avg [i_rank] += val;
				global_tot_avg         += val;
			}
			local_tot_avg[i_rank] /= static_cast<double>( nr_threads );
		}

		global_tot_avg /= static_cast<double>( nr_threads * nr_ranks );

		for( int i_timer(0); i_timer<nr_timers ; ++i_timer )
		{

			std::vector<double> total;

			std::string descr( i_timer < nr_generics_timers ?
							   worker_thread_parameters_[0].generics_timers_[i_timer                     ].description() :
							   worker_thread_parameters_[0].schedule_timers_[i_timer - nr_generics_timers].description()
							 );

			for( int i_rank(0); i_rank<nr_ranks ; ++i_rank)
			{
				std::vector<double> timings(nr_threads);

				for( int i_thread(0); i_thread<nr_threads; ++i_thread)
				{
					timings[i_thread] = global_timings[i_rank][ i_timer * nr_threads + i_thread];
					total.push_back ( global_timings[i_rank][ i_timer * nr_threads + i_thread] );
				}

				std::cout<<table::generate_column( descr
								                 , timings
								                 , local_tot_avg[i_rank] )<<std::endl;
			}

			if( nr_ranks > 1 )
			{
				//std::cout<<table::generate_line()<<std::endl;
				std::cout<<table::generate_column( std::string("glob. ") + descr
											     , total
											     , global_tot_avg )<<std::endl;
			}
			std::cout<<table::generate_line()<<std::endl;
		}

		std::cout<<table::generate_footer()<<std::endl;

		comm.barrier();

	}
	else
	{
		std::vector<double> local_timings( acquire_timings() );

		comm.sendMessg( reinterpret_cast<char*>(&(local_timings.front())), local_timings.size() * sizeof(double), 0 );

		comm.barrier();
	}
}

int
Thread_environment::getNThreads () const
{
  return nr_threads_;
}

}     // namespace scheduler
