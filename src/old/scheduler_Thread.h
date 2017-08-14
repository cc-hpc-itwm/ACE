/** \file
 *  \author Daniel Gruenewald, Leo Nesemann
 *  \date 2011-11-30, rewrite 2013-03
 */
#ifndef SCHEDULER_THREAD_H
#define SCHEDULER_THREAD_H 1

#include "Timer.h"
#include "scheduler/scheduler_Schedule.h"

#include "MCTP2.h"

#include <vector>
#include <sstream>
#include <string>

namespace scheduler {

/** \brief thread-local metadata
 *
 * This struct keeps some metadata for each single thread (ID, state, assigned schedule etc.).
 * It is passed to the worker thread function workerThreadFunc() as a void pointer.
 */
struct Thread
{
    public:
      /** \brief state of a thread (also used to pass "init" and "stop" messages from the master thread) */
      enum State {   IDLE         ///< denotes that the thread is currently not executing a %schedule
                   , INIT_WORK    ///< set the state from "IDLE" to "INIT_WORK" after setting
                                  ///  a %schedule to initialise the computation
                   , WORKING      ///< denotes that the thread is currently executing the previously set
                                  ///  %schedule
                   , EXCEPTION	  ///< denotes that the thread has run into an exception
                   , STOP_THREAD  ///< set this state from the master thread to stop the worker thread as
                                  ///  part of the shutdown process; this will not interrupt currently
                                  ///  executing computations
                   , STOPPED      ///< denotes that the thread's worker function has returned
                 };
    public:
      Thread( int thread_ID
    		, bool pin_thread
    		, State state
    		, Schedule* schedule
    		, mctp2* thread_pool
    		, mctp2* master_thread_pool );

      void
      process_schedule();

//      void
//      start_timer(unsigned int timer_ID);
//
//      void
//      stop_timer(unsigned int timer_ID);

      unsigned int
      pin();

    private:
      int thread_ID_;
      bool pin_thread_;
      State state_;
      Schedule* schedule_;
      mctp2* thread_pool_;
      mctp2* master_thread_pool_;
      std::vector<RTM::Timer> schedule_timers_;
      std::vector<RTM::Timer> generics_timers_;

      std::string exceptinfo_;

      friend class Thread_environment;
};

}   // namespace scheduler

#endif    // SCHEDULER_THREAD_H
