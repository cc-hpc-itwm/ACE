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
 * LockGuard.cpp
 *
 */

#include <ACE/utility/LockGuard.h>

namespace ace {

LockGuard
  ::LockGuard(thread::Mutex &mutex)
: _mutex(mutex)
{
  _mutex.lock();
}

LockGuard::~LockGuard()
{
  _mutex.unlock();
}

}  // namespace ace

