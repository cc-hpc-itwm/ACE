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
 * Optional.hpp
 *
 */

#ifndef OPTIONAL_HPP_
#define OPTIONAL_HPP_

#include <memory>
#include <stdexcept>

namespace scheduler {

template<typename T>
class Optional {

public:

  Optional()
  : pvalue()
  , given(false)
  {}

  Optional( Optional<T> const & A)
  : pvalue( A.given ? std::unique_ptr<T>( new T( *A.pvalue ) )
                    : std::unique_ptr<T>() )
  , given( A.given )
  { }

  explicit
  Optional(const T& _v)
  : pvalue( new T(_v) )
  , given(true)
  {}

  Optional&
  operator = (const Optional<T>& A)
  {
    pvalue = A.given ? std::unique_ptr<T>( new T( *A.pvalue ) )
                     : std::unique_ptr<T>();

    given = A.given;

    return *this;
  }

  Optional&
  operator = (const T& _v)
  {
    pvalue = std::unique_ptr<T>( new T( _v ) );

    given = true;

    return *this;
  }

   operator bool() const
   {
     return given;
   }

  operator T() const
  {
    if( !given )
    {
      throw std::runtime_error("parameter not given");
    }
    return *pvalue;
  }


  T* operator ->()
  {
    if( !given )
    {
      throw std::runtime_error("parameter not given");
    }
    return pvalue.get();
  }

  const T* operator ->() const
  {
    if( !given )
    {
      throw std::runtime_error("parameter not given");
    }
    return pvalue.get();
  }

private:

  std::unique_ptr<T> pvalue;
  bool given;
};

}

#endif /* OPTIONAL_HPP_ */
