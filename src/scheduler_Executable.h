/** \file
 * \author Leo Nesemann
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_EXECUTABLE_H
#define SCHEDULER_EXECUTABLE_H 1

#include "scheduler_TimeDirection.h"
//#include "Timer.h"

#include <vector>


namespace scheduler {


class Executable
{
    public:
      virtual void execute( int timestamp, TimeDirection::Type time_direction ) = 0;
      virtual ~Executable()
      {  }

      virtual unsigned int timer_ID() const = 0;
};


class ExecutableList
{
    public:
      ExecutableList()
      {  }

      void insert(Executable* executable)
      {
        executables_.push_back(executable);
      }

      virtual ~ExecutableList()
      {
        for (size_t i(0); i < executables_.size(); ++i)
        {
          delete executables_[i];
        }
      }

      void execute( int timestamp, TimeDirection::Type time_direction
             /*     , std::vector<RTM::Timer>& thread_timers */)
      {
        for (size_t i(0); i < executables_.size(); ++i)
        {
//          unsigned int const timer_ID(executables_[i]->timer_ID());
//          if (timer_ID < thread_timers.size())
//          {
//            thread_timers[timer_ID].start();
//          }

          executables_[i]->execute(timestamp, time_direction);

//          if (timer_ID < thread_timers.size())
//          {
//            thread_timers[timer_ID].stop();
//          }
        }
      }

    private:
      // non-copyable:
      ExecutableList(ExecutableList const&);
      ExecutableList& operator=(ExecutableList const&);

      std::vector<Executable*> executables_;
};


}   // namespace scheduler


#endif    // SCHEDULER_EXECUTABLE_H
