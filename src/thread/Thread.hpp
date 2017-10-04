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

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <stdexcept>
#include <string>
#include <utility/Optional.hpp>
#include <set>

namespace scheduler {

class Thread {

public:

  //! Type of the function pointers used by the ThreadPool class
  typedef void (*funcptr_t)(int, void*);

  Thread( int id
        , Optional<int> const & core2pin = Optional<int>() );

  virtual ~Thread();

  void
  execute( funcptr_t function
         , void      *arguments );

  void
  wait();

  static std::set<int>
  active_coreset();

private:

  static void * main(void*);

  //! the thread ID
  int _id;

  //! Flags that indicate which threads are working
  //! or supposed to be working (they are suspended if they
  //! are not working)
  bool _workFlag;
  //! The function each thread should call on the next wakeup.
  funcptr_t _workFunction;
  //! The arguments that are passed to each thread when new work is assigned
  void *_workArgument;

  //! Mutexes to protect the _workFlags
  pthread_mutex_t _workMutexStart;
  //! Mutexes to protect the _workFlags
  pthread_mutex_t _workMutexEnd;
  //! Conditions used to signal a thread that it should start working
  pthread_cond_t _workCondStart;
  //! Conditions used to signal a waiting thread that
  //! the current work has been finished
  pthread_cond_t _workCondEnd;

  //! What string of exception raised by a thread function
  std::string _workExceptionWhat;
  //! Flags to indicate whether or not a thread function raised an exception
  bool _didThrow;
  //! Flag that indicates if the threads should shut down on the next wakeup
  bool _shutdown;

  //! The thread handle
  pthread_t _pthread_handle;
};

} /* namespace scheduler */

#endif /* THREAD_H_ */
