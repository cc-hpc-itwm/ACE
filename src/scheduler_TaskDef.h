/*
 * scheduler_TaskDef.h
 *
 *  Created on: 23.08.2013
 *      Author: gruenewa
 */

#ifndef SCHEDULER_TASKDEF_H_
#define SCHEDULER_TASKDEF_H_

#include "scheduler_PreCondition.h"
#include "scheduler_Executable.h"
#include "scheduler_PostCondition.h"

#include <memory>

namespace scheduler {

template <typename State>
class TaskDef
{

	public:

		TaskDef
		  ()
		: pre_conditions_ ()
		, executables_    ()
		, post_conditions_()
		{}

		~TaskDef() {};

		// insert a pre-condition
		void
		insert
		  ( std::unique_ptr<PreCondition<State> > preCondition )
		{
			if(static_cast<bool>(preCondition)) {

				if(!static_cast<bool>(pre_conditions_)) {
					pre_conditions_.reset( new PreConditionList<State> );
				}

				pre_conditions_->insert( std::move(preCondition) );
			}
		}

		bool
		hasPreCondition
		  () const {
		  return static_cast<bool>(pre_conditions_);
		}

		inline PreCondition<State> const &
        getPreCondition
          () const {
          return *pre_conditions_;
        }

        inline PreCondition<State> &
        getPreCondition
          () {
          return *pre_conditions_;
        }

		// insert an executable
		void
		insert
		  ( std::unique_ptr<Executable<State> > executable )
		{
			if(static_cast<bool>(executable)) {

				if( !static_cast<bool>(executables_) ) {
					executables_.reset(new ExecutableList<State>);
				}

				executables_->insert( std::move(executable) );
			}
		}

		bool
        hasExecutable
          () const {
          return static_cast<bool>(executables_);
        }

        inline Executable<State> const &
        getExecutable
          () const {
          return *executables_;
        }

        inline Executable<State> &
        getExecutable
          () {
          return *executables_;
        }

		// insert a post-condition
		void
		insert
		  ( std::unique_ptr<PostCondition<State> > postCondition  ) {

			if(static_cast<bool>(postCondition)) {

				if( !static_cast<bool>(post_conditions_) ) {
					post_conditions_.reset(new PostConditionList<State>);
				}

				post_conditions_->insert( std::move(postCondition) );

			}
		}

		bool
        hasPostCondition
          () const {
          return static_cast<bool>(post_conditions_);
        }

        inline PostCondition<State> const &
        getPostCondition
          () const {
          return *post_conditions_;
        }

        inline PostCondition<State> &
        getPostCondition
          () {
          return *post_conditions_;
        }

		bool
		empty()
		{
		  return ( not(hasPreCondition()) &&
		           not(hasExecutable())   &&
		           not(hasPostCondition()) );
		}

	public:

		std::unique_ptr<PreConditionList<State>>  pre_conditions_;
		std::unique_ptr<ExecutableList<State>>    executables_;
		std::unique_ptr<PostConditionList<State>> post_conditions_;

};

}

#endif /* SCHEDULER_TASKDEF_H_ */
