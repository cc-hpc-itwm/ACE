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
 * ScheduleExecuter.h
 *
 */

#ifndef SCHEDULEEXECUTER_H_
#define SCHEDULEEXECUTER_H_

#include <scheduler_Schedule.h>
#include <ThreadPool.hpp>
#include <Timer.hpp>
#include <vector>

namespace scheduler {

template <typename State>
class ScheduleExecuter {

public:

  ScheduleExecuter
    ( Schedule<State> &
    , ThreadPool & );

  void
  execute();

private:

  static void
  execute
    (int, void *);

  struct t_parameter {

      Schedule<State> & schedule;
  };

  Schedule<State> & _schedule;
  ThreadPool & _pool;

  std::vector<Timer> _totalRunTimer;
  std::vector<Timer> _spinLockTimer;
  std::vector<Timer> _executerTimer;
  std::vector<Timer> _postCondTimer;
};

} /* namespace scheduler */

#include <ScheduleExecuter.cpp>

#endif /* SCHEDULEEXECUTER_H_ */
