/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef TASK_POSTCONDITION_H
#define TASK_POSTCONDITION_H 1

#include <memory>
#include <vector>

namespace scheduler {
namespace task {

template <typename State>
class PostCondition
{
  public:

    virtual
    ~PostCondition
      () {
    }

    virtual void
    set(State const &) = 0;
};

template <typename State>
class PostConditionList
    : public PostCondition<State>
{
  public:

    using ElementType = std::unique_ptr<PostCondition<State> >;

    PostConditionList
      () {
    }

    PostConditionList
      (PostConditionList const&) = delete;

    virtual
    ~PostConditionList() {
    }

    PostConditionList&
    operator=(PostConditionList const&) = delete;

    void
    insert
    (ElementType condition) {
      conditions_.push_back(std::move(condition));
    }

    virtual void
    set(State const & state) {
      for (auto &i: conditions_) {
        i->set(state);
      }
    }

  private:

    std::vector<ElementType> conditions_;
};

} // namespace task
} // namespace scheduler


#endif    // SCHEDULER_CONDITION_H
