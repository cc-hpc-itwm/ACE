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
    delete (*iter);   // members executable_ and (if present) condition_ are deleted in a Task destructor.
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
    LockGuard lock(_mutex);

    if ( tasklist_.end() == spin_ ) {
      spin_ = tasklist_.begin();
    }
    // here: spin_ is a valid, nonsingular iterator.

    typedef typename Tasklist::iterator Iter;

    Iter const old_spin(spin_);

    do {
      ++spin_;
      if ( tasklist_.end() == spin_ ) {
        spin_ = tasklist_.begin();
      }
    } while (   ((*spin_)->status() != task::Task<State>::FREE)
             && (spin_ != old_spin) );

    if ((*spin_)->status() != task::Task<State>::FREE) {
      // no Task with status FREE could be found, so all of them are IN_USE
      // or FINISHED.
      return nullptr;
    }

    (*spin_)->status() = task::Task<State>::IN_USE;
    task_in_use = *spin_;
    // lock goes out of scope here
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
    return 0;
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
      task_in_use->status() = ( task_in_use->finished()
                              ? task::Task<State>::FINISHED
                              : task::Task<State>::FREE );
      // loop on, try again in the next loop
    }

    // usleep(1000);
    /// \todo something like usleep() could be inserted somewhere if the
    ///       scheduler goes into a temporary spin lock
  }

}

}  // namespace schedule
}  // namespace ace
