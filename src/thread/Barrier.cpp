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
 * Barrier.cpp
 *
 */

#include <pthread.h>
#include <thread/Barrier.hpp>
#include <thread/Macros.hpp>

namespace ace {
namespace thread {

Barrier
  ::Barrier
   (unsigned short nThreads)
: _barrier()
{
  PTHREAD_CHECK
    (pthread_barrier_init(&_barrier, NULL, nThreads));
}

Barrier
  ::~Barrier
      ()
{
  pthread_barrier_destroy(&_barrier);
}

void
Barrier
  ::apply
   ()
{
  pthread_barrier_wait(&_barrier);
}

} // namespace thread
} // namespace ace
