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

#include <ACE/device/opencl/MemoryResource.hpp>

#include <ACE/utility/Macros.hpp>

#include <stdexcept>
#include <memory>
#include <numa.h>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <typeindex>
#include <iostream>

namespace ace {
namespace device {
namespace opencl {

MemoryResource
  ::MemoryResource
    ( Type const & type
    , Id const & id ) throw()
{ }

MemoryResource
  ::MemoryResource
    ( MemoryResource const & other ) throw()
{ }

MemoryResource
  ::~MemoryResource()
{ }

void *
MemoryResource
  ::do_allocate
   ( std::size_t bytes
   , std::size_t /*alignment*/)
{
  if ( bytes == 0 ) {
    return NULL;
  }
}

void
MemoryResource
  ::do_deallocate
   ( void* g_ptr
   , std::size_t bytes
   , std::size_t /*alignment*/ )
{
   if(g_ptr == NULL) {
     return;
   }
}

bool
MemoryResource
  ::do_is_equal( const device::MemoryResource& other ) const noexcept
{
  if( typeid(*this) != typeid(other) )
    return false;

  return operator==(dynamic_cast<const MemoryResource&>(other) );
}

bool
MemoryResource
  ::operator==( const MemoryResource& other ) const noexcept
{
  return true;
}

}
}
}
