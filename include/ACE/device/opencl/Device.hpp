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

#ifndef DEVICE_OPENCL_DEVICE_HPP_
#define DEVICE_OPENCL_DEVICE_HPP_

#include <ACE/device/Device.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/device/opencl/MemoryResource.hpp>
#include <CL/cl.hpp>

#include <tuple>

namespace ace {
namespace device {
namespace opencl {

class Device
    : public device::Device {

public:

  Device
    ( Type const & type
    , Id const & id );

  cl::Kernel
  buildKernel
    ( std::string const & functionCode
    , std::string const & functionName );

  template <typename ...Args>
  cl::Kernel
  getBoundKernel
    ( std::string const & functionCode
    , std::string const & functionName
    , Args const & ...args);

  cl::CommandQueue &
  getQueue() {
    return _queue;
  }

private:

  Device
    ( std::tuple<cl::Context, cl::CommandQueue> && tuple );

  cl::Context      _context;
  cl::CommandQueue _queue;

};


}
}
}


/// implementation

namespace ace {
namespace device {
namespace opencl {

namespace detail {


template<typename T>
inline typename std::enable_if
  < !std::is_pointer<T>::value
  , T const &
  >::type
_getKernelParameter(MemoryResource & /*memResource*/, const T & arg) {
  return arg;
}

template<typename T>
inline  typename std::enable_if
  < std::is_pointer<T>::value
  , std::remove_pointer<MemoryResource::DeviceBuffer::Pointer>::type const &
  >::type
_getKernelParameter
  ( MemoryResource & memResource
  , T const & hPointer ) {
  MemoryResource::DeviceBuffer::Pointer dPointer;
  MemoryResource::DeviceBuffer::Offset  dOffset;

  memResource.getOpenCLBufferAndOffset
  (hPointer, dPointer, dOffset);

  return *dPointer;
}

inline void
_setKernelParameters
  ( cl::Kernel &
  , MemoryResource &
  , int
  )
{}

template<typename T, typename... Args>
inline void
_setKernelParameters
  ( cl::Kernel &kernel
  , MemoryResource & memResource
  , int i
  , const T &firstParameter
  , const Args& ...restOfParameters)
{
  kernel.setArg
    ( i
    , _getKernelParameter(memResource,firstParameter)
    );

  _setKernelParameters(kernel,memResource,i+1,restOfParameters...);
}

template<typename... Args>
inline void
setKernelParameters
  ( cl::Kernel &kernel
  , MemoryResource & memResource
  , const Args& ...args )
{
  _setKernelParameters(kernel, memResource, 0, args...);
}

} // namespace detail


template <typename ...Args>
cl::Kernel
Device
  ::getBoundKernel
    ( std::string const & functionCode
    , std::string const & functionName
    , Args const & ...args)
{
  cl::Kernel kernel(buildKernel(functionCode,functionName));

  detail::setKernelParameters
    ( kernel
    , *reinterpret_cast<MemoryResource*>(allocator().resource())
    , args...
    );

  return kernel;
}

}
}
}

#endif /* DEVICE_OPENCL_DEVICE_HPP_ */
