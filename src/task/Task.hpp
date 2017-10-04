/** \file
 * \author Leo Nesemann, ITWM
 * \date 2012-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H 1

#include <memory>
#include <task/Executable.hpp>
#include <task/PostCondition.hpp>
#include <task/PreCondition.hpp>
#include <task/TaskDef.hpp>

#define CACHELINESIZE 64

namespace ace {
namespace task {

template <typename State = int>
class Task
    : public TaskDef<State> {

public:

  enum Status {FREE, IN_USE, FINISHED};

  Task
    ( State const & initialState
    , State const & finalState )
  : TaskDef<State>()
  , state_(initialState)
  , final_(finalState)
  , status_(FREE)
  { }

  ~Task
    () {
  }

  inline void
  execute
    () {
    if(TaskDef<State>::hasExecutable()) {
       TaskDef<State>::getExecutable().execute(state_);
    }
  }

  inline void
  setPostCondition() {
    if( TaskDef<State>::hasPostCondition() ) {
        TaskDef<State>::getPostCondition().set(state_);
    }

    status_ = ( finished()
              ? FINISHED
              : FREE );
  }

  inline bool
  ready_to_execute
    () const
  {
    return ( not(TaskDef<State>::hasPreCondition()) ||
                (TaskDef<State>::getPreCondition().check(state_)) );
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
            >= final_ );
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
  alignas(CACHELINESIZE) State final_;

  Status status_;
};


} // namespace task
} // namespace ace


#endif    // SCHEDULER_TASK_H
