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

#include <ACE/device/numa/MemoryResource.hpp>

#include <ACE/utility/Macros.hpp>

#include <stdexcept>
#include <memory>
#include <numa.h>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <iostream>

namespace ace {
namespace device {
namespace numa {

MemoryResource
  ::MemoryResource
    ( std::size_t node ) throw()
: _pnodemask(NULL)
, _numa_available(true)
{
  if( numa_available() < 0 ) {
    if( node != 0 ) {
      std::stringstream os;
      os << "NUMA not available: node "
         << node;
      throw std::runtime_error(CODE_ORIGIN + os.str());
    }
    _numa_available = false;
  }
  else {
    std::size_t const max_node
      ( static_cast<std::size_t>(numa_max_node()) );

    if( node > max_node ) {
      std::stringstream os;
      os << "Invalid NUMA node "
         << node
         << ": Node is not in available range [0,"
         << (max_node + 1)
         << ")";
      throw std::runtime_error(CODE_ORIGIN + os.str());
    }

    if( max_node == 0 ) {
      _numa_available = false;
    }
    else {
      _pnodemask = numa_allocate_nodemask();

      numa_bitmask_clearall(_pnodemask);

      numa_bitmask_setbit(_pnodemask, node);
    }

  }
}

MemoryResource
  ::MemoryResource
    ( std::set<int> const & coreset ) throw()
: _pnodemask(NULL)
, _numa_available(true)
{
  if( numa_available() < 0 ) {
    _numa_available = false;
  }
  else {
    std::size_t const max_node
      ( static_cast<std::size_t>(numa_max_node()) );

    if( max_node == 0 || coreset.empty() ) {
      _numa_available = false;
    }
    else {
      _pnodemask = numa_allocate_nodemask();

      numa_bitmask_clearall(_pnodemask);

      typedef std::set<int>::iterator Iter;

      for( Iter iter(coreset.begin())
         ;      iter!=coreset.end()
         ;    ++iter ) {
        numa_bitmask_setbit(_pnodemask, numa_node_of_cpu(*iter));
      }
    }
  }
}

MemoryResource
  ::MemoryResource
    ( MemoryResource const & other ) throw()
: _pnodemask(NULL)
, _numa_available(other._numa_available)
{
  _pnodemask = numa_allocate_nodemask();

  copy_bitmask_to_bitmask(other._pnodemask, _pnodemask);
}

MemoryResource
  ::~MemoryResource()
{
  if(_numa_available) {
    numa_free_nodemask(_pnodemask);
  }
}

void *
MemoryResource
  ::do_allocate
   ( std::size_t bytes
   , std::size_t /*alignment*/)
{
  if ( bytes == 0 ) {
    return NULL;
  }

  if( _numa_available ) {
    numa_set_strict(1);
    void * const g_ptr
      (numa_alloc_interleaved_subset
          (bytes,_pnodemask) );
    return g_ptr;
  } else {
    void * const g_ptr(malloc(bytes));
    return g_ptr;
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

   if( _numa_available ) {
     return numa_free(g_ptr,bytes);
   } else {
     return free(g_ptr);
   }
}

bool
MemoryResource
  ::do_is_equal( const device::MemoryResource& other ) const noexcept
{ }

}
}
}
