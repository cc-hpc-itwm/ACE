/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_SCHEDULE_H
#define SCHEDULER_SCHEDULE_H 1

#include <list>
#include <task/Task.hpp>
#include <thread/Mutex.hpp>

namespace scheduler {

/** \brief A schedule, managing executables and conditions when they can be run
 *
 * This class contains a list of executables. These can be added to the schedule with insert().
 * The method get_executable_Task() will return the next task that is allowed to run. To actually
 * run a Schedule, create an instance of scheduler::Threading, set the schedule and call
 * scheduler::Threading::beginComputation() and scheduler::Threading::endComputation().
 *
 * When a task is inserted into the schedule, the schedule takes ownership. As soon as the
 * schedule goes out of scope, the tasks will be destroyed, and with them the executables and
 * conditions. This allows implementations to simply state:
 * \code
 *   scheduler::Schedule init_schedule;
 *   init_schedule.insert( new scheduler::Task( 0, t_final, Time_Direction::FORWARD
 *                                            , new elastic::StencilHasOldTimestamp(x, y, z)
 *                                            , new elastic::Compute(x, y, z) )
 *                       );
 * \endcode
 *
 * \note It is advisable to separate the schedule's construction and execution phase: First,
 *       insert all tasks; then, run the schedule.
 */
template <typename State>
class Schedule
{
	public:

//	  struct Priority {
//		  enum Type {LOW,HIGH};
//	  };

    public:
      /** \brief initialise the schedule */
      Schedule();

      /** \brief cleanup all tasks in the schedule, including conditions and executables */
      virtual
      ~Schedule();

      /** \brief insert another work package into the schedule */
      void
      insert
        (task::Task<State>* task /*, Priority::Type priority = Priority::LOW*/ );

      /** \brief retrieve an executable task
       *
       * This method loops over the whole schedule. If a task is found whose condition evaluates \b true ,
       * this task is returned.
       *
       * \returns A pointer to the next task that should be executed; a NULL pointer if all tasks in the
       *          schedule have been executed and there is nothing more to do.
       * \note If no task in the schedule can currently be executed, this method spins over all tasks until
       *       one of the conditions changes to \b true .
       * \todo Why should the full Task* be returned? Return an Executable* instead?
       */
      task::Task<State> *
      get_executable_Task();

    private:
      typedef std::list<task::Task<State>*>  Tasklist;

    private:

      Tasklist tasklist_;
      typename Tasklist::iterator spin_;

      thread::Mutex _mutex;
};

}   // namespace scheduler

#include <scheduler_Schedule.cpp>

#endif    // SCHEDULER_SCHEDULE_H
