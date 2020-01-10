/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2019
 * 
 * This file is part of ACE.
 * 
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * GaspiLS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * Macros.hpp
 *
 */

#include <stdexcept>
#include <sstream>

namespace ace {
namespace thread {

namespace macro_detail {

struct CodeOriginString : public std::string
{
  CodeOriginString (const char* s, const int i) : std::string ()
  {
    std::ostringstream oss;
    oss << s << " [" << i << "]: ";

    assign (oss.str());
  }
};

}

// macro definitions

#define CODE_ORIGIN macro_detail::CodeOriginString (__PRETTY_FUNCTION__, __LINE__)


//! An exception for failed Pthread API calls
class PthreadError
    : public std::exception {

  public:

    PthreadError
      (std::string const & errMsg = std::string())
    : errMsg_(errMsg)
  {}

  virtual const char*
  what
    () const throw() {
    return errMsg_.c_str();
  }

  private:

    std::string errMsg_;

};

} // namespace thread
} // namespace scheduler

// Macros that convert error codes to exceptions.
// We use macros instead of functions here to avoid unnecessary
// function calls, since these checks are used very often, even
// in performance critical parts of the code.
#define PTHREAD_CHECK(X)                                                       \
  if((X) != 0) {                                                               \
    throw ace::thread::PthreadError(CODE_ORIGIN);                              \
  }
