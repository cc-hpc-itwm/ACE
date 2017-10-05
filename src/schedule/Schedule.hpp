/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2017
 *
 * This file is part of ACE.
 *
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 *
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * Schedule.hpp
 *
 */

#ifndef SCHEDULER_SCHEDULE_H
#define SCHEDULER_SCHEDULE_H 1

#include <list>
#include <task/Task.hpp>
#include <thread/Mutex.hpp>

namespace ace {
namespace schedule {

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
class Schedule {

    public:
      /** \brief initialise the schedule */
      Schedule();

      /** \brief cleanup all tasks in the schedule, including conditions and executables */
      virtual
      ~Schedule();

      /** \brief insert another work package into the schedule */
      void
      insert
        (task::Task<State>* task);

      task::Task<State> *
      getAndLockNextFreeTask
        ();

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

      Tasklist tasklist_;
      typename Tasklist::iterator spin_;

      thread::Mutex _mutex;
};

}  // namespace schedule
}  // namespace ace

#include <schedule/Schedule.cpp>

#endif    // SCHEDULER_SCHEDULE_H
