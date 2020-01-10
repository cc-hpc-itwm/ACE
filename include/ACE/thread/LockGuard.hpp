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
 * Mutex.hpp
 *
 */

#ifndef THREAD_LOCK_GUARD_H_
#define THREAD_LOCK_GUARD_H_

#include <ACE/thread/Mutex.hpp>

namespace ace {

namespace thread {

class LockGuard {

public:

  LockGuard
    (Mutex &);

  ~LockGuard
    ();

private:

  Mutex & _mutex;

};

} /* namespace thread */

} /* namespace ace */

#endif /* THREAD_LOCK_GUARD_H_ */
