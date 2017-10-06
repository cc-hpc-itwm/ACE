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

#include <limits>
#include <schedule/Executor.hpp>

namespace ace {
namespace schedule {

template <typename State>
ScheduleExecuter<State>
  ::ScheduleExecuter
   ( Schedule<State> & schedule
   , thread::Pool & pool)
: _schedule(schedule)
, _pool(pool)
, _totalRunTimer(_pool.numThreads())
, _spinLockTimer(_pool.numThreads())
, _executerTimer(_pool.numThreads())
, _postCondTimer(_pool.numThreads())
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

  double const max(std::numeric_limits<double>::max());
  double min_spinLock(+max); double min_totalRun(+max);
  double min_executer(+max); double min_postCond(+max);

  double max_spinLock(-max); double max_totalRun(-max);
  double max_executer(-max); double max_postCond(-max);

  double avg_spinLock(0.); double avg_totalRun(0.);
  double avg_executer(0.); double avg_postCond(0.);

  for(int tid ( 0 )
     ;    tid < _pool.numThreads()
     ;  ++tid ) {
    double const thread_spinLock(_spinLockTimer[tid].elapsedCycles());
    double const thread_executer(_executerTimer[tid].elapsedCycles());
    double const thread_postCond(_postCondTimer[tid].elapsedCycles());
    double const thread_totalRun(_totalRunTimer[tid].elapsedCycles());

    min_spinLock = std::min(min_spinLock,thread_spinLock);
    max_spinLock = std::max(max_spinLock,thread_spinLock);
    avg_spinLock += thread_spinLock / static_cast<double>(_pool.numThreads());

    min_postCond = std::min(min_postCond,thread_postCond);
    max_postCond = std::max(max_postCond,thread_postCond);
    avg_postCond += thread_postCond / static_cast<double>(_pool.numThreads());

    min_executer = std::min(min_executer,thread_executer);
    max_executer = std::max(max_executer,thread_executer);
    avg_executer += thread_executer / static_cast<double>(_pool.numThreads());

    min_totalRun = std::min(min_totalRun,thread_totalRun);
    max_totalRun = std::max(max_totalRun,thread_totalRun);
    avg_totalRun += thread_totalRun / static_cast<double>(_pool.numThreads());
  }

  std::cout << "schedule execution statistics:" << std::endl;
  std::cout << "\ttotal run time : "
            << avg_totalRun
            << " ["
            << min_totalRun
            << ", "
            << max_totalRun
            << "]\t <-> "
            << ( ( avg_totalRun / avg_totalRun ) * 100. )
            << "%"
            << std::endl;

  std::cout << "\texecuter time : "
            << avg_executer
            << " ["
            << min_executer
            << ", "
            << max_executer
            << "]\t <-> "
            << ( ( avg_executer / avg_totalRun ) * 100. )
            << "%"
            << std::endl;

  std::cout << "\tpostCond time : "
            << avg_postCond
            << " ["
            << min_postCond
            << ", "
            << max_postCond
            << "]\t <-> "
            << ( ( avg_postCond / avg_totalRun ) * 100. )
            << "%"
            << std::endl;

  std::cout << "\tspin lock time (m) : "
            << avg_spinLock
            << " ["
            << min_spinLock
            << ", "
            << max_spinLock
            << "]\t <-> "
            << ( ( avg_spinLock / avg_totalRun ) * 100. )
            << "%"
            << std::endl;

  double const avg_spinLockC
    (avg_totalRun - avg_executer - avg_postCond);
  double const min_spinLockC
    (min_totalRun - min_executer - min_postCond);
  double const max_spinLockC
    (max_totalRun - max_executer - max_postCond);

  std::cout << "\tspin lock time (c): "
            << avg_spinLockC
            << " ["
            << min_spinLockC
            << ", "
            << max_spinLockC
            << "]\t <-> "
            << ( ( avg_spinLockC / avg_totalRun ) * 100. )
            << "%"
            << std::endl;

  std::cout << std::endl;

}

template <typename State>
void
ScheduleExecuter<State>
  ::execute
   ( int tid
   , void * arg ) {

  Schedule<State> & schedule
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_schedule );

  Timer & totalRunTimer
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_totalRunTimer[tid] );

  Timer & executerTimer
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_executerTimer[tid] );

  Timer & postCondTimer
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_postCondTimer[tid] );

  Timer & spinLockTimer
    ( reinterpret_cast<ScheduleExecuter<State> *>(arg)->_spinLockTimer[tid] );

  bool finished(false);

  totalRunTimer.start();
  spinLockTimer.start();

  while (!finished)
  {
    task::Task<State>* const task( schedule.get_executable_Task() );

    if ( task )
    {
      spinLockTimer.stop();

      executerTimer.start();
      task->execute();
      executerTimer.stop();

      postCondTimer.start();
      task->setPostCondition();
      postCondTimer.stop();

      spinLockTimer.start();
    }
    else
    {
      finished = true;
    }
  }

  spinLockTimer.stop();
  totalRunTimer.stop();
}

} // namespace schedule
} // namespace ace
