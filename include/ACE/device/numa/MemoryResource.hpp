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
 * along with GaspiLS. If not, see <http://www.gnu.org/licenses/>.
 *
 * Memory.hpp
 *
 */

#ifndef DEVICE_NUMA_MEMORY_HPP_
#define DEVICE_NUMA_MEMORY_HPP_

#include <ACE/device/MemoryResource.hpp>

#include <numa.h>
#include <set>

namespace ace {
namespace device {
namespace numa  {

class MemoryResource : public device::MemoryResource {

public:

  MemoryResource
    ( std::size_t  node );

  MemoryResource
    ( std::set<int> const & coreset ) throw();

  MemoryResource
      ( MemoryResource const & other ) throw();

  ~MemoryResource();

  void *
  do_allocate
    ( std::size_t bytes
    , std::size_t alignment ) override;

  void
  do_deallocate
    ( void* p
    , std::size_t bytes
    , std::size_t alignment ) override;

  void
  do_host_update
    ( void * const p
    , std::size_t const & size ) override;

  void
  do_device_update
    ( void * const p
    , std::size_t const & size ) override;

  bool
  do_is_equal
  ( const device::MemoryResource& other ) const noexcept override;

  bool
  operator==
    (const MemoryResource& other) const noexcept;

private:

  MemoryResource();
  MemoryResource & operator=(MemoryResource const &);

  bitmask * _pnodemask;
  bool _numa_available;

};

}
}
}

#endif /* DEVICE_NUMA_MEMORY_HPP_ */
