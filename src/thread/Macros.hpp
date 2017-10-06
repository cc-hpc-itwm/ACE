/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2017
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

namespace ace {
namespace thread {

//! An exception for failed Pthread API calls
class PthreadError
    : public std::exception {

  virtual const char*
  what
    () const throw() {
    return "An error occurred during a call to the Pthreads API!";
  }

};

} // namespace thread
} // namespace scheduler

// Macros that convert error codes to exceptions.
// We use macros instead of functions here to avoid unnecessary
// function calls, since these checks are used very often, even
// in performance critical parts of the code.
#define PTHREAD_CHECK(X)                                                       \
  if((X) != 0) {                                                               \
    throw ace::thread::PthreadError();                                   \
  }
