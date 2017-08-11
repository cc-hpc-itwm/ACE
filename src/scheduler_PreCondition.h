/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_PRECONDITION_H
#define SCHEDULER_PRECONDITION_H 1

#include "scheduler_State.h"

#include <vector>
#include <cstring>

namespace scheduler {

template <typename State>
class PreCondition
{

  public:

    virtual bool
    check
      ( State const &) const = 0;

};

template <typename State>
class PreConditionList : public PreCondition<State>
{
    public:

      PreConditionList()
      {  }

      void
      insert
        (PreCondition<State> const* condition)
      {
        conditions_.push_back(condition);
      }

      virtual
      ~PreConditionList()
      {
        for (size_t i(0)
            ;       i < conditions_.size()
            ;     ++i )
        {
          delete conditions_[i];
        }
      }

      virtual bool
      check
        ( State const & state) const
      {
        // no short-circuit, each condition is checked!
        bool retval(true);

        for (size_t i(0)
            ;       i < conditions_.size()
            ; ++i)
        {
          if (! conditions_[i]->check(state) )
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

      std::vector<PreCondition<State> const*> conditions_;
};

}   // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
