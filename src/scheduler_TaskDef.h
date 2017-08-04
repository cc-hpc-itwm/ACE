/*
 * scheduler_TaskDef.h
 *
 *  Created on: 23.08.2013
 *      Author: gruenewa
 */

#ifndef SCHEDULER_TASKDEF_H_
#define SCHEDULER_TASKDEF_H_

#include "scheduler/scheduler_PreCondition.h"
#include "scheduler/scheduler_Executable.h"
#include "scheduler/scheduler_PostCondition.h"

namespace scheduler {

class TaskDef
{

	public:

		TaskDef()
		: pre_conditions_ ( NULL )
		, executables_    ( NULL )
		, post_conditions_( NULL )
		{}

		~TaskDef() {};

		// insert a pre-condition
		void
		insert( PreCondition * const pPreCondition )
		{
			if(pPreCondition != NULL )
			{

				if( pre_conditions_ == NULL )
				{
					pre_conditions_ = new PreConditionList;
				}

				pre_conditions_->insert( pPreCondition );

			}
		}

		// insert an executable
		void
		insert( Executable * const pExecutable )
		{
			if(pExecutable != NULL )
			{

				if( executables_ == NULL )
				{
					executables_ = new ExecutableList;
				}

				executables_->insert( pExecutable );

			}
		}

		// insert a post-condition
		void
		insert( PostCondition * const pPostCondition )
		{
			if(pPostCondition != NULL )
			{

				if( post_conditions_ == NULL )
				{
					post_conditions_ = new PostConditionList;
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

		PreConditionList * pre_conditions_;
		ExecutableList   * executables_;
		PostConditionList* post_conditions_;

};

}

#endif /* SCHEDULER_TASKDEF_H_ */
