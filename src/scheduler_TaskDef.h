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

namespace scheduler {

template <typename State>
class TaskDef
{

	public:

		TaskDef
		  ()
		: pre_conditions_ ( new PreConditionList<State> )
		, executables_    ( new ExecutableList<State> )
		, post_conditions_( new PostConditionList<State> )
		{}

//		TaskDef
//		  ( PreCondition<State> const* pre_condition_par
//		  , ExecutableList<State>* executable_list_par
//		  , PostCondition<State> const* post_condition_par )
//		: pre_conditions_(pre_condition_par)
//	    , executables_(executable_list_par)
//	    , post_conditions_(post_condition_par)
//		{  }

		~TaskDef() {};

		// insert a pre-condition
		void
		insert
		  ( PreCondition<State> * const pPreCondition )
		{
			if(pPreCondition != NULL )
			{

				if( pre_conditions_ == NULL )
				{
					pre_conditions_ = new PreConditionList<State>;
				}

				pre_conditions_->insert( pPreCondition );

			}
		}

		// insert an executable
		void
		insert
		  ( Executable<State> * const pExecutable )
		{
			if(pExecutable != NULL )
			{

				if( executables_ == NULL )
				{
					executables_ = new ExecutableList<State>;
				}

				executables_->insert( pExecutable );

			}
		}

		// insert a post-condition
		void
		insert( PostCondition<State> * const pPostCondition )
		{
			if(pPostCondition != NULL )
			{

				if( post_conditions_ == NULL )
				{
					post_conditions_ = new PostConditionList<State>;
				}

				post_conditions_->insert( pPostCondition );

			}
		}

		bool
		empty()
		{
		  return ( pre_conditions_  == NULL &&
		           executables_     == NULL &&
		           post_conditions_ == NULL );
		}

	public:

		PreConditionList<State> * pre_conditions_;
		ExecutableList<State>   * executables_;
		PostConditionList<State>* post_conditions_;

};

}

#endif /* SCHEDULER_TASKDEF_H_ */
