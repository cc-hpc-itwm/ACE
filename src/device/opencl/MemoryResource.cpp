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
  ::MemoryResource
    ( Type const & type
    , Id const & id ) throw()
: _blocks()
, _mutex()
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
   , std::size_t /*alignment*/)
{
  if ( bytes == 0 ) {
    return nullptr;
  }

  thread::LockGuard lock(_mutex);

  // do the device allocation;
  DeviceBuffer    deviceBuffer    ({new uint8_t[bytes], bytes });
  HostMemoryBlock hostMemoryBlock ({new uint8_t[bytes], bytes });

  auto ret (_blocks.insert
              (std::make_pair
                 ( hostMemoryBlock
                 , deviceBuffer
                 )
               )
           );

  if(ret.second) {
    return hostMemoryBlock.pointer;
  }

  // Not enough free memory!
  throw std::bad_alloc();
}

void
MemoryResource
  ::do_deallocate
   ( void* g_ptr
   , std::size_t bytes
   , std::size_t /*alignment*/ )
{
   if(g_ptr == nullptr) {
     return;
   }

   auto const iter (_blocks.find(HostMemoryBlock({g_ptr,bytes})));

   if(iter==_blocks.end())
   {
     throw std::runtime_error(CODE_ORIGIN + "Allocation not found");
   }

   // delete host memory
   delete[] iter->first.pointer;
   // delete device memory
   delete[] iter->second.pointer;

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

  memcpy
    ( static_cast<void*>(hPointer)
    , static_cast<void*>(static_cast<uint8_t*>(dPointer)+offset)
    , size );

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

  memcpy
    ( static_cast<void*>(static_cast<uint8_t*>(dPointer)+offset)
    , static_cast<void*>(hPointer)
    , size );
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

 auto const iter (_blocks.find(HostMemoryBlock({g_ptr,0})));

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

  auto const iter (_blocks.find(HostMemoryBlock({p,0})));

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
