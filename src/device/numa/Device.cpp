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

#include <ACE/device/numa/Device.hpp>

#include <ACE/device/numa/Scheduler.hpp>
#include <ACE/utility/Macros.hpp>


namespace ace {
namespace device {
namespace numa {

Device
  ::Device
   ( Id const & id )
: device::Device
  ( std::make_unique<MemoryResource>(id)
  , std::make_unique<Scheduler<int>>(id) )
{ }

}
}
}
