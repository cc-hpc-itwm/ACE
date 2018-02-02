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
 * Barrier.hpp
 *
 */

#ifndef THREAD_BARRIER_H_
#define THREAD_BARRIER_H_

#include <pthread.h>

namespace ace {
namespace thread {

class Barrier {

public:

  Barrier
    (unsigned short nThreads);

  ~Barrier
    ();

  void
  apply
    ();

private:

  pthread_barrier_t _barrier;

};

} // namespace thread
} // namespace ace

#endif /* THREAD_BARRIER_H_ */
