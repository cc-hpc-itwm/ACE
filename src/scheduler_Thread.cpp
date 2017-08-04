/** \file
 * \author Leo Nesemann, Daniel Grünewald, ITWM
 * \date 2013-03
 * \copyright Fraunhofer ITWM
 */
#include "scheduler_Thread.h"
#include "scheduler_Timers.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>

#ifdef PAPI_USE_PAPI
#include "papi.h"
#include "Lock.h"
#include "utils/Macros.hpp"

#include "include/defs.h"
#include LOGGERINCLUDE
namespace {

  LOGGER;

} 
#endif

namespace scheduler {

Thread
	::Thread( int thread_ID
    		, bool pin_thread
    		, State state
    		, Schedule* schedule
    		, mctp2* thread_pool
    		, mctp2* master_thread_pool )
: thread_ID_(thread_ID)
, pin_thread_(pin_thread)
, state_(state)
, schedule_(schedule)
, thread_pool_(thread_pool)
, master_thread_pool_(master_thread_pool)
{  }

void
Thread
	::process_schedule()
{
	try
	{
		if( !generics_timers_.empty() )
		{
			generics_timers_[Timers::TOTAL].start();
			generics_timers_[Timers::SPINLOCK].start();
		}

		state_ = WORKING;

		bool finished(NULL == schedule_);

#ifdef PAPI_USE_PAPI
                static const int papi_num_events = 3;
                {
                  int papi_events[papi_num_events] = {PAPI_SP_OPS, PAPI_VEC_SP, PAPI_TOT_CYC};
                  Lock lock(Lock::Papi());
                  int error = PAPI_start_counters(papi_events, papi_num_events);
                  if (error != PAPI_OK) {
                    MESSAGE("PAPI_start_counters error: %s", PAPI_strerror(error));
                    throw std::runtime_error(CODE_ORIGIN + "Could not initialize PAPI counters");
                  }
                }
#endif

		if( !generics_timers_.empty() )
		{
			while (!finished)
			{
				Task* const task( schedule_->get_executable_Task() );

				if ( task )
				{
					generics_timers_[Timers::SPINLOCK].stop();
					task->execute(schedule_timers_);
					generics_timers_[Timers::SPINLOCK].start();
				}
				else
				{
					finished = true;
				}
			}
		}
		else
		{
			while (!finished)
			{
				Task* const task( schedule_->get_executable_Task() );

				if ( task )
				{
					task->execute(schedule_timers_);
				}
				else
				{
					finished = true;
				}
			}
		}

#ifdef PAPI_USE_PAPI
                {
                  long_long results[papi_num_events];
                  int error;
                  {
                    Lock lock(Lock::Papi());
                    error = PAPI_stop_counters(results, papi_num_events);
                  }
                  if (error != PAPI_OK) {
                    MESSAGE("PAPI_stop_counters error: %s",  PAPI_strerror(error));
                  } else {
                    MESSAGE("PAPI counter results: %lld ~ %lld ~ %lld", results[0], results[1], results[2]);
                  }
                }
#endif

		state_ = IDLE;

		if( !generics_timers_.empty() )
		{
			generics_timers_[Timers::SPINLOCK].stop();
			generics_timers_[Timers::TOTAL].stop();
		}

	}
	catch (std::exception& except)
	{
		if( !generics_timers_.empty() )
		{
			generics_timers_[Timers::SPINLOCK].stop();
			generics_timers_[Timers::TOTAL].stop();
		}
		exceptinfo_ = except.what();
		state_ = EXCEPTION;
	}
}


//void
//Thread
//	::start_timer(unsigned int timer_ID)
//{
//	if (timer_ID < timers_.size())
//	{
//		schedule_timers_[timer_ID].start();
//	}
//}
//
//void
//Thread
//	::stop_timer(unsigned int timer_ID)
//{
//	if (timer_ID < timers_.size())
//	{
//		schedule_timers_[timer_ID].stop();
//	}
//}

unsigned int
Thread
	::pin()
{
	/// determine cpu mask for active process
	cpu_set_t process_mask;
	CPU_ZERO(&process_mask);
	sched_getaffinity( 0								// pid 0 corresponds to active process
			         , sizeof(process_mask)
			         , &process_mask);

	unsigned int nCoresMax ( sizeof(cpu_set_t) * 8 ); 	//cpu_set is a bit field
	unsigned int nCoresAct ( 0 );						//active core number

	unsigned int iCore( 0 );
	bool isPinned (false);
	unsigned int usedCore( 0 );

	while( (iCore < nCoresMax) and not( isPinned ) )
	{
		if(CPU_ISSET(iCore, &process_mask))
		{
			if( nCoresAct == thread_ID_ )
			{
				mctp2Thread2CoreAffinity(iCore);

				usedCore = iCore;
				isPinned = true;
			}
			++nCoresAct;
		}
		++iCore;
	}

	if( not(isPinned) )
	{
		std::stringstream ss;

		ss << "Thread "<<thread_ID_<<" could not be pinned!"<<std::endl;
		throw std::runtime_error(ss.str());
	}

	return usedCore;
}


}   // namespace scheduler
