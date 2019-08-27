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

#ifndef DEVICE_SCHEDULER_INTERFACE_HPP_
#define DEVICE_SCHEDULER_INTERFACE_HPP_

//http://www.goldsborough.me/cpp/2018/05/22/00-32-43-type_erasure_for_unopinionated_interfaces_in_c++/

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include <ACE/device/Types.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/utility/Macros.hpp>


namespace ace {
namespace device {

template <typename State>
class SchedulerInterface {

public:

    virtual Type
    device_type() const = 0;

    virtual void
    execute
      (schedule::Schedule<State> & schedule) = 0;

    virtual std::size_t
    numManagedResources
      () const = 0;

};

}
}

#endif /* DEVICE_SCHEDULER_INTERFACE_HPP_ */
