/** \file
 * \author Leo Nesemann, ITWM
 * \date 2012-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H 1

#include "scheduler_PreCondition.h"
#include "scheduler_Executable.h"
#include "scheduler_PostCondition.h"

#include "scheduler_TaskDef.h"

namespace scheduler {


template <typename State = int>
class Task : public TaskDef<State>
{
    public:

      enum Status {FREE, IN_USE, FINISHED};

    public:
      Task( State const & initialState
          , State const & finalState
//          , PreCondition<State> const* pre_condition_par
//          , ExecutableList<State>* executable_list_par
//          , PostCondition<State> const* post_condition_par = NULL
          )
        : TaskDef<State>()
        , state_(initialState)
        , final_(finalState)
        , status_(FREE)
        , pre_condition_
            (TaskDef<State>::pre_conditions_)
        , executable_
            (TaskDef<State>::executables_)
        , post_condition_
            (TaskDef<State>::post_conditions_)
//        , pre_condition_(pre_condition_par)
//        , executable_(executable_list_par)
//        , post_condition_(post_condition_par)
      {  }

//      Task( State const & initialState
//          , State const & finalState
//          , PreCondition<State> const* pre_condition_par
//          , Executable<State>* executable_par
//          , PostCondition<State> const* post_condition_par = NULL
//          )
//        : state_(initialState)
//        , final_(finalState)
//        , status_(FREE)
//        , pre_condition_(pre_condition_par)
//        , executable_(new ExecutableList<State>)
//        , post_condition_(post_condition_par)
//      {
//        executable_list_->insert(executable_par);
//      }

      ~Task()
      {
        if (pre_condition_)
        {
          delete pre_condition_;
        }

        delete executable_;

        if (post_condition_)
        {
        	delete post_condition_;
        }
      }

      void execute(/*std::vector<RTM::Timer>& thread_timers*/)
      {
        executable_->execute(state_);
        if( post_condition_ )
        {
        	post_condition_->set(state_);
        }
//        ++state_;

        status_ = (  finished()  ?  FINISHED
                                 : FREE      );
      }

      bool
      ready_to_execute() const
      {
        return ( not(pre_condition_) ||
                 (pre_condition_->check(state_)) );
      }

      State &
      state()
      {
        return state_;
      }

      State const &
      state() const
      {
        return state_;
      }

      bool
      finished() const
      {
        return (   state_
                >= final_ );
      }

      Status
      status
        () const
      {
        return status_;
      }

      Status&
      status
        ()
      {
        return status_;
      }

      PreCondition<State> &
      pre_condition()
      {
        return *pre_condition_;
      }

      Executable<State> &
      executable()
      {
        return *executable_;
      }

      PostCondition<State> &
      post_condition()
      {
    	  return *post_condition_;
      }

    private:

      State state_;
      State final_;

      Status status_;

      PreCondition<State>  * pre_condition_;
      Executable<State>    * executable_;
      PostCondition<State> * post_condition_;
};


}   // namespace scheduler


#endif    // SCHEDULER_TASK_H
