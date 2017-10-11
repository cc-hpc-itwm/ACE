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
 * Schedule.cpp
 *
 */
#include <schedule/Schedule.hpp>
#include <utility/LockGuard.h>

namespace ace {
namespace schedule {

template <typename State>
Schedule<State>
	::Schedule()
  :  tasklist_()
{ }

template <typename State>
Schedule<State>
	::~Schedule()   // is virtual
{
  typedef typename Tasklist::iterator Iter;

  for (Iter iter (tasklist_.begin())
      ;     iter != tasklist_.end()
      ;   ++iter ) {
    delete (*iter);   // members executable_ and (if present)
                      // condition_ are deleted in a Task destructor.
  }
}


template <typename State>
void
Schedule<State>
	::insert(task::Task<State>* task)
{
  LockGuard lock(_mutex);

  tasklist_.insert(tasklist_.begin(), task);
}

template <typename State>
typename Schedule<State>::iterator
Schedule<State>
  ::begin
   () {
  return tasklist_.begin();
}

template <typename State>
task::Task<State> *
Schedule<State>
  ::getAndLockNextFreeTask
   (iterator & spin_) {

  task::Task<State>* task_in_use(nullptr);
  {
    if ( tasklist_.end() == spin_ ) {
      spin_ = tasklist_.begin();
    }
    // here: spin_ is a valid, nonsingular iterator.

    iterator const old_spin(spin_);

    do {
      ++spin_;
      if ( tasklist_.end() == spin_ ) {
        spin_ = tasklist_.begin();
      }

      using Status = typename task::Task<State>::Status;

      if((*spin_)->status() == Status::FREE) {

        Status expected(Status::FREE);
        Status desired (Status::IN_USE);

        int * const pExpectedL(reinterpret_cast<int*>(&expected));
        int * const pDesiredL (reinterpret_cast<int*>(&desired));
        int * const pValueL   (reinterpret_cast<int*>(&(*spin_)->status()));

        if(__sync_bool_compare_and_swap(pValueL, *pExpectedL, *pDesiredL)) {
          task_in_use = *spin_;
          break;
        }
      }

    } while ( spin_ != old_spin );

    // no Task with status FREE could be found, so all of them are IN_USE
    // or FINISHED. So, we  return nullptr;
  }

  return task_in_use;

}

template <typename State>
task::Task<State>*
Schedule<State>
	::get_executable_Task(iterator & spin)
{
  // assumption here: tasklist_ is not modified concurrently when getExecutable() is called
  /// \todo make clear documentation about concurrency assumptions here
  if (tasklist_.empty())
  {
    return nullptr;
  }

  while(true)
  {

    task::Task<State>* task_in_use
      ( getAndLockNextFreeTask(spin) );

    if( !task_in_use) {
      return task_in_use;
    }

    if (task_in_use->ready_to_execute())
    {
      return task_in_use;
    }
    else
    {
      using Status = typename task::Task<State>::Status;

      task_in_use->status() = ( task_in_use->finished()
                              ? Status::FINISHED
                              : Status::FREE );
      // loop on, try again in the next loop
    }

    // usleep(1000);
    /// \todo something like usleep() could be inserted somewhere if the
    ///       scheduler goes into a temporary spin lock
  }

}

}  // namespace schedule
}  // namespace ace
