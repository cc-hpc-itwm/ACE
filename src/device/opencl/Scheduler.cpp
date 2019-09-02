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
 * Scheduler.cpp
 *
 */

#include <ACE/device/opencl/Scheduler.hpp>

#include <ACE/utility/Macros.hpp>


namespace ace {
namespace device {
namespace opencl {

namespace detail {

}

SchedulerRuntime
  ::SchedulerRuntime
   ( Type const & type, Id const & id )
: _pThreadPool
    ( std::make_shared<thread::Pool>
        ( 1, thread::PIN_1TO1_INHERITED) )
{ }

}
}
}
