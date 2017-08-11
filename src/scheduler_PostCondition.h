/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_POSTCONDITION_H
#define SCHEDULER_POSTCONDITION_H 1

#include "scheduler_State.h"

#include <vector>


namespace scheduler {

template <typename State>
class PostCondition
{
    public:
      virtual void
      set(State const &) = 0;
};

template <typename State>
class PostConditionList
    : public PostCondition<State>
{
    public:
      PostConditionList()
        {  }

      void insert(PostCondition<State> * condition)
      {
        conditions_.push_back(condition);
      }

      virtual
      ~PostConditionList()
      {
        for (size_t i(0); i < conditions_.size(); ++i)
        {
          delete conditions_[i];
        }
      }

      virtual void
      set(State const & state)
      {
        // no short-circuit, each condition is checked!
        for (size_t i(0); i < conditions_.size(); ++i)
        {
          conditions_[i]->set(state);
        }
      }

    private:
      // non-copyable:
      PostConditionList (PostConditionList const&);
      PostConditionList& operator=(PostConditionList const&);

      std::vector<PostCondition<State> *> conditions_;
};

}   // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
