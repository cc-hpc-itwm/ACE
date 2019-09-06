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
 * MemoryResource.hpp
 *
 */

#ifndef DEVICE_MEMORYRESOURCE_HPP_
#define DEVICE_MEMORYRESOURCE_HPP_

#include <cstddef>

namespace ace {
namespace device {

class MemoryResource {

public:
    MemoryResource()          = default;
    virtual ~MemoryResource() = default;

    void*
    allocate
      ( std::size_t bytes
      , std::size_t alignment = alignof(std::max_align_t)
      );

    void
    deallocate
      ( void* p
      , std::size_t bytes
      , std::size_t alignment = alignof(std::max_align_t)
      );

    bool
    is_equal
      ( const MemoryResource& other ) const noexcept;


    void
    updateHost
      ( void * const p
      , std::size_t const & size );

    void
    updateDevice
      ( void * const p
      , std::size_t const & size );

    friend bool
    operator==
      ( const MemoryResource& a
      , const MemoryResource& b
      );


    friend bool
    operator!=
      ( const MemoryResource& a
      , const MemoryResource& b
      );

private:

    virtual void*
    do_allocate
      ( std::size_t bytes
      , std::size_t alignment
      ) = 0;

    virtual void
    do_deallocate
      ( void* p
      , std::size_t bytes
      , std::size_t alignment
       ) = 0;

    virtual void
    do_host_update
      ( void * const p
      , std::size_t const & size ) = 0;

    virtual void
    do_device_update
      ( void * const p
      , std::size_t const & size ) = 0;

    virtual bool
    do_is_equal
      ( const MemoryResource& other ) const noexcept = 0;

};

}
}

#endif /* MEMORYRESOURCE_HPP_ */
