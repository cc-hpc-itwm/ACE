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
 * Allocator.hpp
 *
 */

#ifndef DEVICE_ALLOCATOR_HPP_
#define DEVICE_ALLOCATOR_HPP_

#include <ACE/device/MemoryResource.hpp>

#include <type_traits>

namespace ace {
namespace device {

template <typename T = void>
class Allocator
{
  public:

    template <typename U> friend struct Allocator;

    using value_type = T;
    using pointer = T *;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    explicit
    Allocator
      ( MemoryResource * const resource )
    : _resource(resource)
    {}

    template <typename U>
    Allocator
      ( Allocator<U> const & rhs )
    : _resource(rhs.resource())
    {}

    pointer
    allocate
      ( std::size_t n )
    {
      return static_cast<pointer>
        (_resource->allocate(n * sizeof(T),alignof(T)));
    }

    void
    deallocate
      ( pointer p
      , std::size_t n)
    {
        _resource->deallocate(p, n * sizeof(T), alignof(T));
    }

    template <typename U>
    bool
    operator==
      (Allocator<U> const & rhs) const
    {
      return (*(this->resource())) == (*rhs.resource());
    }

    template <typename U>
    bool
    operator!=
      (Allocator<U> const & rhs) const
    {
      return !(*this == rhs);
    }

    MemoryResource * const
    resource() const {
      return _resource;
    }

private:

    MemoryResource * const _resource;

};

}
}

#endif /* DEVICE_ALLOCATOR_HPP_ */
