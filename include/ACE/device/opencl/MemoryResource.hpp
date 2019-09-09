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

#ifndef DEVICE_OPENCL_MEMORY_HPP_
#define DEVICE_OPENCL_MEMORY_HPP_

#include <ACE/device/MemoryResource.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/thread/Mutex.hpp>

#include <map>

namespace cl { class Buffer; class Context; class CommandQueue; }

namespace ace {
namespace device {
namespace opencl  {

class MemoryResource : public device::MemoryResource {

public:

  struct HostMemBlock
  {
     using Pointer = void *;
     using Size = std::size_t;

     static Pointer
     allocate
       ( std::size_t bytes
       , std::size_t alignment );

     static void
     deallocate
       ( Pointer const & p
       , std::size_t bytes
       , std::size_t alignment );

     bool
     operator<
       ( HostMemBlock const & other ) const;

     Pointer     pointer;
     std::size_t size;
  };

  struct DeviceBuffer
  {
#ifdef EMULATE_DEVICE
     using Pointer = void *;
#else
     using Pointer = cl::Buffer *;
#endif
     using Offset  = std::size_t;
     using Size    = std::size_t;

     static Pointer
     allocate
       ( std::size_t bytes
       , std::size_t alignment
       , MemoryResource & resource );

     static void
     deallocate
       ( Pointer const & p
       , std::size_t bytes
       , std::size_t alignment );

     Pointer pointer;
     Size    size;
  };

  MemoryResource
    ( cl::Context & context
    , cl::CommandQueue & queue ) throw();

  MemoryResource
      ( MemoryResource const & other ) = delete;

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

  // transfer data from device to host
  void
  do_host_update
    ( void * const p
    , std::size_t const & size ) override;

   // transfer data from host to device
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

  void
  getOpenCLBufferAndOffset
    ( void * const  p
    , DeviceBuffer::Pointer & pDevice
    , DeviceBuffer::Offset  & pOffset ) const;

  // true if address is contained in current allocations
  // false otherwise
  bool
  hasBeenAllocated
    ( void * const p ) const;

private:

  friend std::ostream&
  operator<<
    ( std::ostream& os
    , const MemoryResource& man );

  MemoryResource();
  MemoryResource & operator=(MemoryResource const &);

  using AllocationMap = std::map<HostMemBlock,DeviceBuffer>;

  AllocationMap _blocks;
  thread::Mutex _mutex;

  cl::Context &      _context;
  cl::CommandQueue & _queue;

};

}
}
}

#endif /* DEVICE_OPENCL_MEMORY_HPP_ */
