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
 * LockGuard.hpp
 *
 */

#ifndef LOCKGUARD_HPP_
#define LOCKGUARD_HPP_

#include <thread/Mutex.hpp>

namespace scheduler {

//! Simple lock guard for Pthread mutexes
class LockGuard
{
public:
  LockGuard(thread::Mutex & mutex);

  ~LockGuard();

private:
  thread::Mutex & _mutex;
};

}

#endif /* LOCKGUARD_HPP_ */
