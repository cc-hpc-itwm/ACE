/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_POSTCONDITION_H
#define SCHEDULER_POSTCONDITION_H 1

#include "scheduler_TimeDirection.h"

#include <vector>


namespace scheduler {

class PostCondition
{
    public:
      virtual void
      set(int timestamp, TimeDirection::Type time_direction) const = 0;
};

class PostConditionList : public PostCondition
{
    public:
      PostConditionList()
        {  }

      void insert(PostCondition const* condition)
      {
        conditions_.push_back(condition);
      }

      virtual ~PostConditionList()
      {
        for (size_t i(0); i < conditions_.size(); ++i)
        {
          delete conditions_[i];
        }
      }

      virtual void set(int timestamp, TimeDirection::Type time_direction) const
      {
        // no short-circuit, each condition is checked!
        for (size_t i(0); i < conditions_.size(); ++i)
        {
          conditions_[i]->set(timestamp, time_direction);
        }
      }

    private:
      // non-copyable:
      PostConditionList (PostConditionList const&);
      PostConditionList& operator=(PostConditionList const&);

      std::vector<PostCondition const*> conditions_;
};

}   // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
