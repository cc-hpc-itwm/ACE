/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2019
 * 
 * This file is part of ACE.
 * 
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * ACE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * Scheduler.hpp
 *
 */

#ifndef DEVICE_OPENCL_SCHEDULER_HPP_
#define DEVICE_OPENCL_SCHEDULER_HPP_

#include <ACE/device/SchedulerInterface.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/schedule/Executor.hpp>
#include <ACE/schedule/Schedule.hpp>

#include <memory>

namespace cl { class CommandQueue; }

namespace ace {
namespace device {
namespace opencl {

class SchedulerRuntime
{

public:

    SchedulerRuntime
      ( cl::CommandQueue & queue );

protected:

    std::shared_ptr<thread::Pool> _pThreadPool;
    cl::CommandQueue &            _queue;
};


//
//
//template <typename State>
//class Scheduler {
//
//public:
//
//    virtual void
//    execute
//      (Schedule<State> & schedule) = 0;
//};
//
//template <typename State>
//class SchedulerFactory {
//
//public:
//    virtual std::unique_ptr<Scheduler<State>>
//    construct(SchedulerRuntime const & ) = 0;
//
//};

template <typename State>
class Scheduler
  : public SchedulerRuntime
  , public device::SchedulerInterface<State> {

public:

    Scheduler
      ( cl::CommandQueue & queue )
    : SchedulerRuntime(queue)
    {}

    template <typename OtherState>
    Scheduler
      (Scheduler<OtherState> const & scheduler)
    : SchedulerRuntime(scheduler)
    {}

    Type
    device_type() const override
    {
      return OPENCL;
    }

    void
    execute
      (schedule::Schedule<State> & schedule) override
    {
      schedule::ScheduleExecuter<State>
        ( const_cast<schedule::Schedule<State> &>
           (schedule)
        ,*_pThreadPool
        ).execute();
    }

    std::size_t
    numManagedResources() const override
    {
      return 1;
    }

};

}
}
}

#endif /* DEVICE_OPENCL_SCHEDULER_HPP_ */
