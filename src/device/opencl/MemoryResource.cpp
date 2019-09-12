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
#include <ACE/thread/LockGuard.hpp>
#include <ACE/utility/Macros.hpp>

#include <CL/cl.hpp>

#include <stdexcept>
#include <memory>
#include <numa.h>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <typeindex>
#include <iostream>
#include <cstring>

namespace ace {
namespace device {
namespace opencl {

MemoryResource
  ::HostMemBlock
     ::Pointer
MemoryResource
  ::HostMemBlock
   ::allocate
      ( std::size_t bytes
      , std::size_t /*alignment*/ )
{
  return static_cast<void*>(new uint8_t[bytes]);
}

void
MemoryResource
  ::HostMemBlock
   ::deallocate
      ( Pointer const & p
      , std::size_t /*bytes*/
      , std::size_t /*alignment*/ )
{
  delete[] static_cast<uint8_t*>(p);
}

bool
MemoryResource
  ::HostMemBlock
   ::operator<
    ( HostMemBlock const & other) const
{
 std::size_t const  diff( size > 0 ? (size-1) : 0 );

 return ( static_cast<void*>
           ( static_cast<uint8_t*>(pointer) + diff )
             < other.pointer );
}

#ifdef EMULATE_DEVICE
MemoryResource
  ::DeviceBuffer
     ::Pointer
MemoryResource
  ::DeviceBuffer
   ::allocate
      ( std::size_t bytes
      , std::size_t /*alignment*/
      , MemoryResource & /*resource*/ )
{
  return static_cast<void*>(new uint8_t[bytes]);
}

void
MemoryResource
  ::DeviceBuffer
   ::deallocate
      ( Pointer const & p
      , std::size_t /*bytes*/
      , std::size_t /*alignment*/ )
{
  delete[] static_cast<uint8_t*>(p);
}
#else
MemoryResource
  ::DeviceBuffer
     ::Pointer
MemoryResource
  ::DeviceBuffer
   ::allocate
      ( std::size_t bytes
      , std::size_t /*alignment*/
      , MemoryResource & resource )
{
  return new cl::Buffer(resource._context, CL_MEM_READ_WRITE, bytes);
}

void
MemoryResource
  ::DeviceBuffer
   ::deallocate
      ( Pointer const & p
      , std::size_t /*bytes*/
      , std::size_t /*alignment*/ )
{
  delete p;
}
#endif

MemoryResource
  ::MemoryResource
    ( cl::Context & context
    , cl::CommandQueue & queue ) throw()
: _blocks()
, _mutex()
, _context(context)
, _queue(queue)
{ }

MemoryResource
  ::~MemoryResource()
{
  if( !_blocks.empty() ) {
    std::stringstream ss;
    ss << "~MemoryManager: Managed memory still in use at destruction"
       << std::endl
       << *this
       << std::endl;
    std::cerr << ss.str() << std::endl;
  }
}

void *
MemoryResource
  ::do_allocate
   ( std::size_t bytes
   , std::size_t alignment)
{
  if ( bytes == 0 ) {
    return nullptr;
  }

  thread::LockGuard lock(_mutex);

  // do the device allocation;
  DeviceBuffer deviceBuffer
    ({DeviceBuffer::allocate(bytes,alignment, *this), bytes });
  HostMemBlock hostMemBlock
    ({HostMemBlock::allocate(bytes,alignment), bytes });

  auto ret (_blocks.insert
              (std::make_pair
                 ( hostMemBlock
                 , deviceBuffer
                 )
               )
           );

  if(ret.second) {
    return hostMemBlock.pointer;
  }

  // Not enough free memory!
  throw std::bad_alloc();
}

void
MemoryResource
  ::do_deallocate
   ( void* g_ptr
   , std::size_t bytes
   , std::size_t alignment )
{
   if(g_ptr == nullptr) {
     return;
   }

   auto iter (_blocks.find(HostMemBlock({g_ptr,bytes})));

   if(iter==_blocks.end())
   {
     throw std::runtime_error(CODE_ORIGIN + "Allocation not found");
   }

   // delete host memory
   HostMemBlock::deallocate(iter->first.pointer , bytes, alignment);
   // delete device memory
   DeviceBuffer::deallocate(iter->second.pointer, bytes, alignment);

   _blocks.erase(iter);
}

void
MemoryResource
  ::do_host_update
    ( void * const hPointer
    , std::size_t const & size )
{
  // transfer data from device to host
  DeviceBuffer::Pointer dPointer;
  DeviceBuffer::Offset  offset;

  getOpenCLBufferAndOffset
    (hPointer, dPointer, offset);

#ifdef EMULATE_DEVICE
  memcpy
    ( static_cast<void*>(hPointer)
    , static_cast<void*>(static_cast<uint8_t*>(dPointer)+offset)
    , size );
#else
  _queue.enqueueReadBuffer(*dPointer, CL_TRUE, offset, size, hPointer);
#endif

}

void
MemoryResource
  ::do_device_update
    ( void * const hPointer
    , std::size_t const & size )
{
  // transfer data from host to device
  DeviceBuffer::Pointer dPointer;
  DeviceBuffer::Offset  offset;

  getOpenCLBufferAndOffset
    (hPointer, dPointer, offset);

#ifdef EMULATE_DEVICE
  memcpy
    ( static_cast<void*>(static_cast<uint8_t*>(dPointer)+offset)
    , static_cast<void*>(hPointer)
    , size );
#else
  _queue.enqueueWriteBuffer(*dPointer, CL_TRUE, offset, size, hPointer);
#endif
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

void
MemoryResource
  ::getOpenCLBufferAndOffset
   ( void * const  g_ptr
   , DeviceBuffer::Pointer & pDevice
   , DeviceBuffer::Offset  & pOffset ) const
{
 if(g_ptr == nullptr) {
   throw std::runtime_error(CODE_ORIGIN);
 }

 auto const iter (_blocks.find(HostMemBlock({g_ptr,0})));

 if(iter==_blocks.end())
 {
   throw std::runtime_error(CODE_ORIGIN + "Allocation not found");
 }

 pDevice = iter->second.pointer;
 pOffset = static_cast<uint8_t*>(g_ptr)
         - static_cast<uint8_t*>(iter->first.pointer);
}

bool
MemoryResource
  ::hasBeenAllocated
    ( void * const p ) const
{
  if(p == nullptr) {
   throw std::runtime_error(CODE_ORIGIN);
  }

  auto const iter (_blocks.find(HostMemBlock({p,0})));

  return (iter!=_blocks.end());
}

std::ostream&
operator<<
  ( std::ostream& os
  , const MemoryResource& man)
{
  using Iter = MemoryResource::AllocationMap::const_iterator;


  int iblock(0);

  os << "OpenCL device allocations: " << std::endl;
  for( Iter it ( man._blocks.begin() )
     ;      it != man._blocks.end()
     ;    ++it) {

    os << "block " << iblock << ": "
       << "host (addr. " << it->first.pointer << ", size "
       << it->first.size << " bytes), "
       << "device buffer ("
       << it->second.pointer
       << ", "
       << it->second.size
       << ")"
       << std::endl;

    ++iblock;
  }

  return os;
}

}
}
}
