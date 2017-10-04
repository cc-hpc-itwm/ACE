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
 * Thread.h
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
