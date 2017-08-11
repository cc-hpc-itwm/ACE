/** \file
 * \author Leo Nesemann
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_EXECUTABLE_H
#define SCHEDULER_EXECUTABLE_H 1

#include "scheduler_State.h"
//#include "Timer.h"

#include <vector>


namespace scheduler {

template <typename State>
class Executable
{
    public:
      virtual void execute( State const & ) = 0;
      virtual ~Executable()
      {  }
};


template <typename State>
class ExecutableList : public Executable<State>
{
    public:
      ExecutableList()
      {  }

      void insert(Executable<State>* executable)
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

      virtual void execute( State const & state
             /*     , std::vector<RTM::Timer>& thread_timers */)
      {
        for (size_t i(0); i < executables_.size(); ++i)
        {
//          unsigned int const timer_ID(executables_[i]->timer_ID());
//          if (timer_ID < thread_timers.size())
//          {
//            thread_timers[timer_ID].start();
//          }

          executables_[i]->execute(state);

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

      std::vector<Executable<State>*> executables_;
};


}   // namespace scheduler


#endif    // SCHEDULER_EXECUTABLE_H
