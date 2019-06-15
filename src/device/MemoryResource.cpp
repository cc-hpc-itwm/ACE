/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2016
 * 
 * This file is part of GaspiLS.
 * 
 * GaspiLS is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GaspiLS. If not, see <http://www.gnu.org/licenses/>.
 *
 * MemoryResource.cpp
 *
 */

#include <ACE/device/MemoryResource.hpp>

namespace ace {
namespace device {

void*
MemoryResource
  ::allocate
    ( std::size_t bytes
    , std::size_t alignment
    ) {
  return this->do_allocate(bytes, alignment);
}

void
MemoryResource
  ::deallocate
    ( void* p
    , std::size_t bytes
    , std::size_t alignment
    ) {
  return this->do_deallocate(p, bytes, alignment);
}

bool
MemoryResource
  ::is_equal
    ( const MemoryResource& other ) const noexcept {
  return this->do_is_equal(other);
}

bool
operator==
    ( const MemoryResource& a
    , const MemoryResource& b) {
  return &a == &b || a.is_equal(b);
}


bool
operator!=
    ( const MemoryResource& a
    , const MemoryResource& b) {
  return !(a == b);
}

}
}
