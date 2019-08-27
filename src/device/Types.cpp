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
 * MemoryResource.cpp
 *
 */

#include <ACE/device/Types.hpp>
#include <ACE/utility/Macros.hpp>

#include <exception>

namespace ace {
namespace device {

std::ostream&
operator<<
  ( std::ostream &os
  , const Type &type )
{
  switch (type) {
    case CPU:  os << "CPU";  break;
    case NUMA: os << "NUMA"; break;
    case GPU:  os << "GPU";  break;
    case FPGA: os << "FPGA"; break;
    default:
      throw std::runtime_error
        (CODE_ORIGIN + "Type not supported yet");
  }

  return os;
}

}
}
