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
 * Executor.hpp
 *
 */

#ifndef SCHEDULEEXECUTER_H_
#define SCHEDULEEXECUTER_H_

#include <ACE/schedule/Schedule.hpp>
#include <ACE/thread/Pool.hpp>
#include <ACE/Timer.hpp>
#include <vector>

namespace ace {
namespace schedule {

template <typename State>
class ScheduleExecuter {

public:

  ScheduleExecuter
    ( Schedule<State> &
    , thread::Pool & );

  void
  execute
    (bool const printTimingInfo = false);

private:

  static void
  execute
    (int, void *);

  struct t_parameter {

      Schedule<State> & schedule;
  };

  Schedule<State> & _schedule;
  thread::Pool & _pool;

  std::vector<Timer> _totalRunTimer;
  std::vector<Timer> _spinLockTimer;
  std::vector<Timer> _executerTimer;
  std::vector<Timer> _postCondTimer;
  std::vector<size_t> _taskExecCounter;
};

} // namespace schedule
} // namespace ace

#include <ACE/schedule/Executor_impl.hpp>

#endif /* SCHEDULEEXECUTER_H_ */
