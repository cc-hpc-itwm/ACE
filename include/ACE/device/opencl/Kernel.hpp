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
 * Kernel.hpp
 *
 */

#ifndef DEVICE_OPENCL_KERNEL_HPP_
#define DEVICE_OPENCL_KERNEL_HPP_

#include <CL/cl.hpp>

namespace ace {
namespace device {
namespace opencl {

class Kernel : public ace::device::Kernel
             , public cl::Kernel {

public:

    Kernel
      (cl::Kernel const & kernel)
    : cl::Kernel(kernel)
    , event_()
    {}

    cl::Event &
    event() {
      return event_;
    }

private:

    cl::Event event_;


};

} // opencl
} // device
} // opencl

#endif /* DEVICE_OPENCL_KERNEL_HPP_ */
