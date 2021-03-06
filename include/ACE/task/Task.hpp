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
 * Task.hpp
 *
 */

#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H 1

#include <memory>
#include <ACE/task/Executable.hpp>
#include <ACE/task/PostCondition.hpp>
#include <ACE/task/PreCondition.hpp>
#include <ACE/task/TaskDef.hpp>

#define CACHELINESIZE 64

namespace ace {
namespace task {

template <typename State = int>
class Task
    : public TaskDef<State> {

public:

  enum class Status : int {FREE = 1, IN_USE = 2, FINISHED = 3};

  Task
    ( State const & initialState
    , State const & finalState )
  : TaskDef<State>()
  , state_(initialState)
  , first_(initialState)
  , final_(finalState)
  , status_(Status::FREE)
  { }

  ~Task
    () {
  }

  inline void
  execute
    () {
    if(TaskDef<State>::hasExecutable()) {
       TaskDef<State>::getExecutable().execute(state_,first_,final_);
    }
  }

  inline void
  setPostCondition() {
    if( TaskDef<State>::hasPostCondition() ) {
        TaskDef<State>::getPostCondition().set(state_,first_,final_);
    }

    status_ = ( finished()
              ? Status::FINISHED
              : Status::FREE );
  }

  inline bool
  ready_to_execute
    () const
  {
    return ( not(TaskDef<State>::hasPreCondition()) ||
                (TaskDef<State>::getPreCondition().check(state_,first_,final_)) );
  }

  inline State &
  state
    () {
    return state_;
  }

  inline State const &
  state
    () const
  {
    return state_;
  }

  inline bool
  finished
    () const {
    return (   state_
            == final_ );
  }

  inline Status const &
  status
    () const {
    return status_;
  }

  inline Status &
  status
    () {
    return status_;
  }

private:

  alignas(CACHELINESIZE) State state_;
  alignas(CACHELINESIZE) State first_;
  alignas(CACHELINESIZE) State final_;

  alignas(CACHELINESIZE) Status status_;
};


} // namespace task
} // namespace ace


#endif    // SCHEDULER_TASK_H
