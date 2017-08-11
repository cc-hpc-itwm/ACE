/** \file
 *  \author Daniel Gruenewald, Leo Nesemann
 *  \date 2011-11-30, rewrite 2013-03
 */
#ifndef SCHEDULER_THREADING_H_
#define SCHEDULER_THREADING_H_ 1

#include "comm/comm_OneSided.h"

#include "scheduler/scheduler_Thread.h"
#include "scheduler/scheduler_Schedule.h"
#include "scheduler/scheduler_Timers.h"
#include "Timer.h"

#include "MCTP2.h"

#include <vector>


namespace scheduler {

/** \brief manage the local parallelisation
 *
 * This class sets up a pool of worker threads. Using the setSchedule() and beginComputation() methods, work
 * can be assigned to this pool and is then executed by the worker threads.
 *
 * A typical program execution looks like this:
 *  - At the program's start, determine the number of worker threads and create an instance of Thread_environment.
 *  - Create the first schedule, fill it with instances of scheduler::Executable.
 *  - Set the schedule with setSchedule(), then execute it with calls to beginComputation() and endComputation().
 *  - Create the next schedule.
 *  - Set the next schedule with setSchedule, then execute it.
 *  - ...
 *
 * The worker thread pool will be closed as the Thread_environment instance goes out of scope.
 *
 * \note The parallelisation is done via MCTP.
 * \note Only a single instance of Thread_environment is allowed due to technical restrictions from the MCTP library.
 * \note If the master thread does not do any work between calls to beginComputation() and endComputation(), the
 *       shorthand execute_schedule() can be used.
 * \note Assumption:\n
 *       In the case of a multisocket system, it is assumed that the process affinity mask is set to include
 *       only the CPU's of one socket (can be achieved by using the mctpSetSocketAffinity function).
 */
class Thread_environment
{
    public:
      Thread_environment(unsigned int nr_threads = 0);
      virtual ~Thread_environment();

      template<class Timer_generator>
      void init_timers();

      void setSchedule(Schedule& schedule);
      void beginComputation();
      void endComputation();
      void execute_schedule(Schedule& schedule);
      void print_timers() const;
      void print_timers( OneSidedComm & comm ) const;

      int getNThreads() const;


    public:
    private:
      static void* workerThreadFunc(void* ptr);
      std::vector<double>
      acquire_timings() const;

    private:
      mctp2 master_thread_pool_;
      mctp2 worker_thread_pool_;
      std::vector<Thread> worker_thread_parameters_;

      int  nr_threads_;
      bool pin_threads_;

      static const int sleepTime_worker_thread_ = 1;    ///< sleep time for each worker thread, in ms
      static const int sleepTime_master_thread_ = 1;    ///< sleep time for the master thread, in ms
      static int allTimeInstances_;
};




template<class Timer_generator>
void Thread_environment::init_timers()
{
  size_t const nr_schedule_timers( Timer_generator::nr_timers() );
  size_t const nr_generics_timers( Timers::nr_timers() );

  for (int thread_id(0); thread_id < nr_threads_; ++thread_id)
  {
	  if( worker_thread_parameters_[thread_id].generics_timers_.size() != nr_generics_timers )
	  {
		  worker_thread_parameters_[thread_id].generics_timers_.clear();
		  worker_thread_parameters_[thread_id].generics_timers_.reserve(nr_generics_timers);
		  for (unsigned int timer_id(0); timer_id < nr_generics_timers; ++timer_id)
		  {
			  worker_thread_parameters_[thread_id].generics_timers_.push_back( RTM::Timer( Timers::descriptions[timer_id] ) );
			  // this already initialises all timers to t=0.
		  }
	  }
	  else
	  {
		  for (unsigned int timer_id(0); timer_id < nr_generics_timers; ++timer_id)
		  {
			  if( worker_thread_parameters_[thread_id].generics_timers_[timer_id].description() !=
				  Timers::descriptions[timer_id] )
			  {
				  throw std::runtime_error("thread timers are initialized already and timer descriptions differ!");
			  }
			  worker_thread_parameters_[thread_id].generics_timers_[timer_id].reset();
		  }
	  }

	  if( worker_thread_parameters_[thread_id].schedule_timers_.size() != nr_schedule_timers )
	  {
		  worker_thread_parameters_[thread_id].schedule_timers_.clear();
		  worker_thread_parameters_[thread_id].schedule_timers_.reserve(nr_schedule_timers);
		  for (unsigned int timer_id(0); timer_id < nr_schedule_timers; ++timer_id)
		  {
			  worker_thread_parameters_[thread_id].schedule_timers_.push_back( RTM::Timer( Timer_generator::descriptions[timer_id] ) );
			  // this already initialises all timers to t=0.
		  }
	  }
	  else
	  {
		  for (unsigned int timer_id(0); timer_id < nr_schedule_timers; ++timer_id)
		  {
			  if( worker_thread_parameters_[thread_id].schedule_timers_[timer_id].description() !=
			  	  Timer_generator::descriptions[timer_id] )
			  {
				  throw std::runtime_error("thread timers are initialized already and timer descriptions differ!");
			  }
			  worker_thread_parameters_[thread_id].schedule_timers_[timer_id].reset();
		  }
	  }
  }
}



}   // namespace scheduler

#endif /* SCHEDULER_THREADING_H_ */
