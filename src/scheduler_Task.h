/** \file
 * \author Leo Nesemann, ITWM
 * \date 2012-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H 1

#include "scheduler_TimeDirection.h"
#include "scheduler_PreCondition.h"
#include "scheduler_Executable.h"
#include "scheduler_PostCondition.h"

namespace scheduler {


class Task
{
    public:
      enum Status {FREE, IN_USE, FINISHED};

    public:
      Task( int t_first_par
          , int t_final_par
          , TimeDirection::Type time_direction_par
          , PreCondition const* pre_condition_par
          , ExecutableList* executable_list_par
          , PostCondition const* post_condition_par = NULL
          )
        : timestamp_(t_first_par)
        , t_final_(t_final_par)
        , nr_timesteps_((t_final_ - timestamp_) * time_direction_par)
        , time_direction_(time_direction_par)
        , status_(FREE)
        , pre_condition_(pre_condition_par)
        , executable_list_(executable_list_par)
        , post_condition_(post_condition_par)
      {  }

      Task( int t_first_par
    	  , int t_final_par
    	  , TimeDirection::Type time_direction_par
          , PreCondition const* pre_condition_par
          , Executable* executable_par
          , PostCondition const* post_condition_par = NULL
          )
        : timestamp_(t_first_par)
        , t_final_(t_final_par)
        , nr_timesteps_((t_final_ - timestamp_) * time_direction_par)
        , time_direction_(time_direction_par)
        , status_(FREE)
        , pre_condition_(pre_condition_par)
        , executable_list_(new ExecutableList)
        , post_condition_(post_condition_par)
      {
        executable_list_->insert(executable_par);
      }

      ~Task()
      {
        if (pre_condition_)
        {
          delete pre_condition_;
        }

        delete executable_list_;

        if (post_condition_)
        {
        	delete post_condition_;
        }
      }

      void execute(/*std::vector<RTM::Timer>& thread_timers*/)
      {
        executable_list_->execute(timestamp_, time_direction_/*, thread_timers*/);
        if( post_condition_ )
        {
        	post_condition_->set(timestamp_, time_direction_);
        }
        timestamp_ += time_direction_;
        status_ = (  finished()  ?  FINISHED
                                 : FREE      );
      }

      bool ready_to_execute() const
      {
        return (    (!pre_condition_)
                 || (pre_condition_->check(timestamp_, time_direction_)) );
      }

      int timestamp() const
      {
        return timestamp_;
      }

      TimeDirection::Type time_direction() const
      {
        return time_direction_;
      }

      bool finished() const
      {
        return (   (time_direction_ * timestamp_)
                >= (time_direction_ * t_final_)    );
      }

      Status status() const
      {
        return status_;
      }

      Status& status()
      {
        return status_;
      }

      PreCondition const* pre_condition() const
      {
        return pre_condition_;
      }

      ExecutableList* executable_list()
      {
        return executable_list_;
      }

      PostCondition const* post_condition() const
      {
    	  return post_condition_;
      }

    private:
      int timestamp_;
      int const t_final_;
      int const nr_timesteps_;
      TimeDirection::Type const time_direction_;
      Status status_;
      PreCondition const* pre_condition_;
      ExecutableList* executable_list_;
      PostCondition const* post_condition_;
};


}   // namespace scheduler


#endif    // SCHEDULER_TASK_H
