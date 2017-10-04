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
 * Thread.cpp
 *
 */

#include <thread/Macros.hpp>
#include <thread/Mutex.hpp>

namespace ace {

namespace thread {

Mutex
  ::Mutex
   ()
:_mutex()
{
  PTHREAD_CHECK
    (pthread_mutex_init(&_mutex, NULL));
}

Mutex
  ::~Mutex
      ()
{
  pthread_mutex_destroy(&_mutex);
}

void
Mutex
  ::lock
      ()
{
  pthread_mutex_lock(&_mutex);
}

void
Mutex
  ::unlock
      ()
{
  pthread_mutex_unlock(&_mutex);
}

} // namespace thread */
} // namespace ace
