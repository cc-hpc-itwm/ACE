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
 * Scheduler.hpp
 *
 */

#ifndef DEVICE_SCHEDULER_HPP_
#define DEVICE_SCHEDULER_HPP_

//http://www.goldsborough.me/cpp/2018/05/22/00-32-43-type_erasure_for_unopinionated_interfaces_in_c++/

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include <ACE/device/SchedulerInterface.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/device/numa/Scheduler.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/utility/Macros.hpp>

namespace ace {
namespace device {

// This is going to be part of the device implementation
template <typename State>
class Scheduler : public SchedulerInterface<State>{

public:

    Scheduler() = delete;

    template <typename OtherState>
    Scheduler
      (SchedulerInterface<OtherState> const & scheduler)
    : _pScheduler(make_copy(&scheduler))
    {}

    Scheduler
      (Scheduler const & scheduler)
    : _pScheduler(make_copy(scheduler._pScheduler.get()))
    {}

    template <typename OtherState>
    Scheduler
      (Scheduler<OtherState> const & scheduler)
    : _pScheduler(make_copy(scheduler._pScheduler.get()))
    {}

    Type
    device_type() const override
    {
      return _pScheduler->device_type();
    }

    void
    execute
      (schedule::Schedule<State> & schedule) override
    {
      return _pScheduler->execute(schedule);
    }

private:

    template <typename OtherState>
    std::unique_ptr<SchedulerInterface<State>>
    make_copy
      (SchedulerInterface<OtherState> const * const pScheduler )
    {
      switch (pScheduler->device_type())
        {
          case Type::NUMA:
            return std::unique_ptr<SchedulerInterface<State>>
                ( new numa::Scheduler<OtherState>
                  ( dynamic_cast<const numa::Scheduler<OtherState>&>
                   (*pScheduler)
                  )
                );
        }

        throw std::runtime_error (CODE_ORIGIN + "failed");
    }

    std::unique_ptr<SchedulerInterface<State>> _pScheduler;

};

}
}

#endif /* DEVICE_SCHEDULER_HPP_ */
