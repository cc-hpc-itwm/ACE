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
 * Mutex.hpp
 *
 */

#ifndef THREAD_MUTEX_H_
#define THREAD_MUTEX_H_

#include <pthread.h>

namespace ace {

namespace thread {

class Mutex {

public:

  Mutex
    ();

  ~Mutex
    ();

  void
  lock
    ();

  void
  unlock
    ();

private:

  pthread_mutex_t _mutex;

};

} /* namespace thread */

} /* namespace ace */

#endif /* THREAD_MUTEX_H_ */
