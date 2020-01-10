/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2019
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
 * PreCondition.hpp
 *
 */

#ifndef TASK_PRECONDITION_H
#define TASK_PRECONDITION_H 1

#include <memory>
#include <vector>
#include <cstring>

namespace ace {
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
      ( State const & state
      , State const & first
      , State const & final ) const  = 0 ;

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
      ( State const & state
      , State const & first
      , State const & final ) const {
      bool retval(true);

      for (auto &i: conditions_) {
        if (! i->check(state,first,final) ) {
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
} // namespace ace


#endif    // SCHEDULER_CONDITION_H
