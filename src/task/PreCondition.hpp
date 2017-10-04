/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef TASK_PRECONDITION_H
#define TASK_PRECONDITION_H 1

#include <memory>
#include <vector>
#include <cstring>

namespace scheduler {
namespace task {

template <typename State>
class PreCondition {

  public:

    virtual
    ~PreCondition
      () {
    }

    virtual bool
    check
      ( State const & state) const  = 0 ;

};

template <typename State>
class PreConditionList
    : public PreCondition<State> {

  public:

    using ElementType = std::unique_ptr<PreCondition<State> >;

    PreConditionList() {
    }

    PreConditionList
      (PreConditionList const&) = delete;

    virtual
    ~PreConditionList
      () {
    }

    PreConditionList&
    operator=
      (PreConditionList const&) = delete;

    void
    insert
      (ElementType condition) {
      conditions_.push_back(std::move(condition));
    }

    virtual bool
    check
      ( State const & state) const {
      bool retval(true);

      for (auto &i: conditions_) {
        if (! i->check(state) ) {
          retval = false;
          break;
        }
      }

      return retval;
    }

  private:

    std::vector<ElementType> conditions_;
};

} // namespace task
} // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
