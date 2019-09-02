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

#include <algorithm>
#include <cctype>
#include <exception>
#include <functional>
#include <map>
#include <string>

namespace ace {
namespace device {

std::ostream&
operator<<
  ( std::ostream &os
  , const Type &type )
{
  switch (type) {
    case CPU:    os << "CPU"   ;  break;
    case NUMA:   os << "NUMA"  ; break;
    case GPU:    os << "GPU"   ;  break;
    case FPGA:   os << "FPGA"  ; break;
    case OPENCL: os << "OPENCL"; break;
    default:
      throw std::runtime_error
        (CODE_ORIGIN + "Type not supported yet");
  }

  return os;
}

std::istream&
operator>>
  ( std::istream &is
  , Type &type )
{
  std::map<std::string,Type> const map {
    std::make_pair("CPU"   ,CPU ),
    std::make_pair("NUMA"  ,NUMA),
    std::make_pair("GPU"   ,GPU ),
    std::make_pair("FPGA"  ,FPGA),
    std::make_pair("OPENCL",OPENCL)
  };

  std::string typestring; is >> typestring;

  std::for_each
    ( typestring.begin()
    , typestring.end()
    , [](char & c){
        c = std::toupper(c);
      }
    );

  auto iter (map.find(typestring));
  if(iter == map.end()) {
    std::stringstream ss;

    ss << "Device type "
       << typestring
       << " is not supported";

    throw std::runtime_error
      (CODE_ORIGIN + ss.str());
  }

  type = iter->second;

  return is;
}

}
}
