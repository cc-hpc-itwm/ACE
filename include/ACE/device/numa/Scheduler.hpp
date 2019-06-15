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

#ifndef DEVICE_NUMA_SCHEDULER_HPP_
#define DEVICE_NUMA_SCHEDULER_HPP_

#include <ACE/device/SchedulerInterface.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/schedule/Executor.hpp>
#include <ACE/schedule/Schedule.hpp>

#include <memory>

namespace ace {
namespace device {
namespace numa {

class SchedulerRuntime
{

public:

    SchedulerRuntime
      ( Id const & id );

protected:

    std::shared_ptr<thread::Pool> _pThreadPool;
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
      ( Id const & id )
    : SchedulerRuntime(id)
    {}


    Type
    device_type() const override
    {
      return NUMA;
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
};

}
}
}

#endif /* DEVICE_NUMA_SCHEDULER_HPP_ */
