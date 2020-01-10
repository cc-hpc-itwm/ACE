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
 * Executor.hpp
 *
 */

#ifndef OPENCL_SCHEDULEEXECUTER_H_
#define OPENCL_SCHEDULEEXECUTER_H_

#include <ACE/schedule/Schedule.hpp>
#include <ACE/thread/Pool.hpp>
#include <ACE/Timer.hpp>
#include <CL/cl.hpp>
#include <vector>

namespace ace {
namespace device {
namespace opencl {

template <typename State>
class ScheduleExecuter {

public:

  ScheduleExecuter
    ( schedule::Schedule<State> &
    , thread::Pool &
    , cl::CommandQueue & queue);

  void
  execute
    (bool const printTimingInfo = false);

private:

  static void
  execute
    (int, void *);

  static void CL_CALLBACK
  setTaskPostCondition
    ( cl_event event, cl_int status, void * voidTask);

  struct t_parameter {

      schedule::Schedule<State> & schedule;
  };

  schedule::Schedule<State> & _schedule;
  thread::Pool & _pool;
  cl::CommandQueue & _queue;

  std::vector<Timer>  _totalRunTimer;
  std::vector<Timer>  _spinLockTimer;
  std::vector<Timer>  _executerTimer;
  std::vector<Timer>  _postCondTimer;
  std::vector<size_t> _taskExecCounter;
};

} // namespace opencl
} // namespace device
} // namespace ace

#include <ACE/device/opencl/Executor_impl.hpp>

#endif /* SCHEDULEEXECUTER_H_ */
