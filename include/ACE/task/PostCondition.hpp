/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2017
 *
 * This file is part of ACE.
 *
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 *
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * PostCondition.hpp
 *
 */

#ifndef TASK_POSTCONDITION_H
#define TASK_POSTCONDITION_H 1

#include <memory>
#include <vector>

namespace ace {
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
} // namespace ace


#endif    // SCHEDULER_CONDITION_H
