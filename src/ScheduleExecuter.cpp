/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2016
 * 
 * This file is part of GaspiLS.
 * 
 * GaspiLS is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GaspiLS. If not, see <http://www.gnu.org/licenses/>.
 *
 * ScheduleExecuter.cpp
 *
 */

#include "ScheduleExecuter.h"

namespace scheduler
{

template <typename State>
ScheduleExecuter<State>
  ::ScheduleExecuter
   ( Schedule<State> & schedule
   , ThreadPool & pool)
: _schedule(schedule)
, _pool(pool)
{}

template <typename State>
void
ScheduleExecuter<State>
  ::execute()
{
  for(int tid ( 0 )
     ;    tid < _pool.numThreads()
     ;  ++tid ) {
    _pool.run
      ( tid
      , execute
      , reinterpret_cast<void*>(this) );
  }

  _pool.waitAll();
}

template <typename State>
void
ScheduleExecuter<State>
  ::execute
   ( int /*tid*/
   , void * arg ) {

  Schedule<State> & schedule
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_schedule );

  bool finished(false);

  while (!finished)
  {
    Task<State>* const task( schedule.get_executable_Task() );

    if ( task )
    {
      task->execute();
    }
    else
    {
      finished = true;
    }
  }
}

} /* namespace scheduler */
