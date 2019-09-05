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
 * MemoryHandle.hpp
 *
 */

#ifndef MEMORY_MEMORYHANDLE_HPP_
#define MEMORY_MEMORYHANDLE_HPP_

#include "ACE/device/Types.hpp"
#include <ACE/utility/Macros.hpp>

#include <iterator>
#include <limits>
#include <stdexcept>


namespace ace {
namespace device {


template <typename T>
class MemoryHandle
{
public:
  // iterator properties
  typedef std::random_access_iterator_tag   iterator_category;
  typedef T                                 value_type;
  typedef std::ptrdiff_t                    difference_type;
  typedef MemoryHandle<T>                   pointer;
  typedef T&                                reference;

  MemoryHandle ();

  MemoryHandle (T* const address);

  MemoryHandle (void* const address);

  MemoryHandle ( T* const address
               , const Type type );

  template<typename T_>
  MemoryHandle (MemoryHandle<T_> const&);


  inline
  T* address () const;

  inline
  Type type () const;

  inline
  T* operator-> () const;

  inline
  T& operator* () const;


  inline
  operator T* () const;


  MemoryHandle& operator++ ();

  MemoryHandle operator++ (int);

  MemoryHandle& operator+= (const std::ptrdiff_t n);

  MemoryHandle& operator-- ();

  MemoryHandle operator-- (int);

  MemoryHandle& operator-= (const std::ptrdiff_t n);


  MemoryHandle& operator= (T* p);


private:

  T*     _address;
  Type   _type;
};


template <typename T>
MemoryHandle<T> operator+ ( const MemoryHandle<T>& p
                          , const std::ptrdiff_t n );

template <typename T>
MemoryHandle<T> operator- ( const MemoryHandle<T>& p
                          , const std::ptrdiff_t n );


template <typename U, typename V>
bool operator== ( const MemoryHandle<U>& lhs
                , const MemoryHandle<V>& rhs );

template <typename U, typename V>
bool operator!= ( const MemoryHandle<U>& lhs
                , const MemoryHandle<V>& rhs );

MemoryHandle<char> memcpy ( const MemoryHandle<char>& destination
                          , const MemoryHandle<char>& source
                          , const std::size_t num );



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * implementation
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
MemoryHandle<T>::MemoryHandle ()
: _address (NULL)
, _type    (CPU)
{}

template <typename T>
MemoryHandle<T>::MemoryHandle (T* const address)
: _address (address)
, _type    (CPU)
{}

template <typename T>
MemoryHandle<T>::MemoryHandle (void* const address)
: _address (reinterpret_cast<T*>(address))
, _type    (CPU)
{}

template <typename T>
MemoryHandle<T>::MemoryHandle ( T* const address
                              , Type const type)
: _address (address)
, _type (type)
{}

template<typename T>
template<typename T_>
MemoryHandle<T>::MemoryHandle (MemoryHandle<T_> const& other)
: _address (reinterpret_cast<T*>(other.address()))
, _type (other.type())
{}


template <typename T>
T* MemoryHandle<T>::address () const
{
  return _address;
}

template <typename T>
Type MemoryHandle<T>::type () const
{
  return _type;
}

template <typename T>
T* MemoryHandle<T>::operator-> () const
{
  return _address;
}

template <typename T>
T& MemoryHandle<T>::operator* () const
{
  return *_address;
}


template <typename T>
MemoryHandle<T>::operator T* () const
{
  return _address;
}


// pointer arithmetics

template <typename T>
MemoryHandle<T>& MemoryHandle<T>::operator++ ()
{
  ++_address;

  return *this;
}

template <typename T>
MemoryHandle<T> MemoryHandle<T>::operator++ (int)
{
  MemoryHandle p (*this);
  ++_address;

  return p;
}

template <typename T>
MemoryHandle<T>& MemoryHandle<T>::operator+= (const std::ptrdiff_t n)
{
  _address += n;

  return *this;
}

template <typename T>
MemoryHandle<T>& MemoryHandle<T>::operator-- ()
{
  --_address;

  return *this;
}

template <typename T>
MemoryHandle<T> MemoryHandle<T>::operator-- (int)
{
  MemoryHandle p (*this);
  --_address;

  return p;
}

template <typename T>
MemoryHandle<T>& MemoryHandle<T>::operator-= (const std::ptrdiff_t n)
{
  _address -= n;

  return *this;
}


template <typename T>
MemoryHandle<T>& MemoryHandle<T>::operator= (T* p)
{
  _address = p;

  return *this;
}


// non-member functions

template <typename T>
MemoryHandle<T> operator+ ( const MemoryHandle<T>& p
                          , const std::ptrdiff_t n )
{
  MemoryHandle<T> res (p);

  return (res += n);
}

template <typename T>
MemoryHandle<T> operator- ( const MemoryHandle<T>& p
                          , const std::ptrdiff_t n )
{
  MemoryHandle<T> res (p);

  return (res -= n);
}


// pointer comparison

template <typename U, typename V>
bool operator== ( const MemoryHandle<U>& lhs
                , const MemoryHandle<V>& rhs )
{
  return (  (static_cast<U*> (lhs) == static_cast<V*> (rhs))
         && (lhs.type()            == rhs.type()           ) );
}

template <typename U, typename V>
bool operator!= ( const MemoryHandle<U>& lhs
                , const MemoryHandle<V>& rhs )
{
  return !operator== (lhs, rhs);
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const MemoryHandle<T>& p)
{
  os << "(" << std::hex << p.address() << std::dec << ", "
     << p.type() << ")";

  return os;
}


}
}


#endif /* MEMORY_MEMORYHANDLE_HPP_ */
