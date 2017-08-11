/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#include "scheduler_Schedule.h"

#include <utility/LockGuard.h>

namespace scheduler {

template <typename State>
Schedule<State>
	::Schedule()
  :  tasklist_()
  ,  spin_(tasklist_.begin())
{ }

template <typename State>
Schedule<State>
	::~Schedule()   // is virtual
{
  typedef typename Tasklist::iterator Iter;

  for (Iter iter (tasklist_.begin())
      ;     iter != tasklist_.end()
      ;   ++iter )
  {
    delete (*iter);   // members executable_ and (if present) condition_ are deleted in a Task destructor.
  }
}


template <typename State>
void
Schedule<State>
	::insert(Task<State>* task)
{
  LockGuard lock(_mutex);

  tasklist_.insert(tasklist_.begin(), task);
}

template <typename State>
Task<State>*
Schedule<State>
	::get_executable_Task()
{
  // assumption here: tasklist_ is not modified concurrently when getExecutable() is called
  /// \todo make clear documentation about concurrency assumptions here
  if (tasklist_.empty())
  {
    return 0;
  }

  while(true)
  {
    Task<State>* task_in_use;
    {
      LockGuard lock(_mutex);

      if ( tasklist_.end() == spin_ )
      {
        spin_ = tasklist_.begin();
      }
      // here: spin_ is a valid, nonsingular iterator.

      typedef typename Tasklist::iterator Iter;

      Iter const old_spin(spin_);

      do
      {
        ++spin_;
        if ( tasklist_.end() == spin_ )
        {
          spin_ = tasklist_.begin();
        }
      } while (   ((*spin_)->status() != Task<State>::FREE)
               && (spin_ != old_spin)             );

      if ((*spin_)->status() != Task<State>::FREE)
      {
        // no Task with status FREE could be found, so all of them are IN_USE or FINISHED.
        return 0;
      }

      (*spin_)->status() = Task<State>::IN_USE;
      task_in_use = *spin_;
      // lock goes out of scope here
    }

    if (task_in_use->ready_to_execute())
    {
      return task_in_use;
    }
    else
    {
      task_in_use->status() = ( task_in_use->finished()
                              ? Task<State>::FINISHED
                              : Task<State>::FREE );
      // loop on, try again in the next loop
    }

    // usleep(1000);
    /// \todo something like usleep() could be inserted somewhere if the scheduler goes into a temporary spin lock
  }

}

}     // namespace scheduler
