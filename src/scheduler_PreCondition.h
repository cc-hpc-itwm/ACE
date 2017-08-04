/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_PRECONDITION_H
#define SCHEDULER_PRECONDITION_H 1

#include "scheduler_TimeDirection.h"

#include <vector>
#include <cstring>

namespace scheduler {

class PreCondition
{
    public:
      virtual bool check(int timestamp, TimeDirection::Type time_direction) const = 0;
};

class PreConditionList : public PreCondition
{
    public:
      PreConditionList()
        {  }

      void insert(PreCondition const* condition)
      {
        conditions_.push_back(condition);
      }

      virtual ~PreConditionList()
      {
        for (size_t i(0); i < conditions_.size(); ++i)
        {
          delete conditions_[i];
        }
      }

      virtual bool
      check( int timestamp
           , TimeDirection::Type time_direction) const
      {
        // no short-circuit, each condition is checked!
        bool retval(true);

        for (size_t i(0); i < conditions_.size(); ++i)
        {
          if (! conditions_[i]->check(timestamp, time_direction) )
          {
            retval = false;
          }
        }

        return retval;
      }

    private:
      // non-copyable:
      PreConditionList (PreConditionList const&);
      PreConditionList& operator=(PreConditionList const&);

      std::vector<PreCondition const*> conditions_;
};

}   // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
