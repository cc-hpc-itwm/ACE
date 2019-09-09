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
 * Device.cpp
 *
 */

#include <ACE/device/opencl/Device.hpp>

#include <ACE/device/opencl/MemoryResource.hpp>
#include <ACE/device/opencl/Scheduler.hpp>
#include <ACE/utility/Macros.hpp>

#include <CL/cl.h>
#include <CL/cl.hpp>

namespace ace {
namespace device {
namespace opencl {

namespace detail {

using TupleType = std::tuple<cl::Context, cl::CommandQueue>;

TupleType
getOpenCLContextAndQueue
  ( Type const & type
  , Id const & id )
{
  std::cout << "Do not forget to implement other combinations" << std::endl;

  cl::Platform platform(cl::Platform::getDefault());

  std::vector<cl::Device> all_devices;

  platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);

  if(all_devices.size()==0){
    std::stringstream ss;

    ss << "No "
       << type
       << " device found!";

    throw std::runtime_error
      (CODE_ORIGIN + ss.str());
  }

  if(all_devices.size()<=id){
    std::stringstream ss;

    ss << "No "
        << type
        << " device with Id "
        << id
        << "found! Valid range is [0.."
        << (all_devices.size()-1)
        << "]";

    throw std::runtime_error
    (CODE_ORIGIN + ss.str());
  }

 cl::Device device(all_devices[id]);

 cl::Context context(device);

 cl::CommandQueue queue(context, device);

 return {std::move(context), std::move(queue)};
}

}

Device
  ::Device
   ( Type const & type, Id const & id )
: Device
  ( detail::getOpenCLContextAndQueue(type,id) )
{ }

Device
  ::Device
   ( detail::TupleType && tuple )
: device::Device
  ( std::make_unique<MemoryResource>
    ( _context
    , _queue
    )
  , std::make_unique<Scheduler<int>>
    ( _queue )
  )
, _context(std::move(std::get<0>(tuple)))
, _queue(std::move(std::get<1>(tuple)))
{ }

}
}
}
