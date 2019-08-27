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
 * Device.hpp
 *
 */

#ifndef DEVICE_DEVICE_HPP_
#define DEVICE_DEVICE_HPP_

#include <ACE/device/Allocator.hpp>
#include <ACE/device/Scheduler.hpp>

#include <memory>

namespace ace {
namespace device {

class Device {

public:

    template <typename T = void>
    Allocator<T>
    allocator() const
    {
      return Allocator<T>(_pMemoryResource.get());
    }


    template <typename State=int>
    Scheduler<State>
    scheduler
      () const
    {
      return Scheduler<State>(_scheduler);
    }

protected:

    Device
      ( std::unique_ptr<MemoryResource> pMemoryResource
      , std::unique_ptr<SchedulerInterface<int>> pScheduler );

private:

    std::unique_ptr<MemoryResource> _pMemoryResource;
    Scheduler<int>                  _scheduler;
};

}
}

#endif /* DEVICE_DEVICE_HPP_ */
