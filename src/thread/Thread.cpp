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

#include <pthread.h>
#include <thread/Exception.hpp>
#include <thread/Macros.hpp>
#include <thread/Thread.hpp>

namespace scheduler {

Thread
  ::Thread
   ( int id
   , Optional<int> const & core2pin )
: _id(id)
, _workFlag(false)
, _workFunction(NULL)
, _workArgument(NULL)
, _workMutexStart()
, _workMutexEnd()
, _workCondStart()
, _workCondEnd()
, _workExceptionWhat()
, _didThrow(false)
, _shutdown(false)
, _pthread_handle()
{
  PTHREAD_CHECK(pthread_mutex_init(&_workMutexStart, NULL));
  PTHREAD_CHECK(pthread_mutex_init(&_workMutexEnd, NULL));
  PTHREAD_CHECK(pthread_cond_init (&_workCondStart, NULL));
  PTHREAD_CHECK(pthread_cond_init (&_workCondEnd, NULL));

  PTHREAD_CHECK(pthread_create(&_pthread_handle,
                               NULL,
                               main,
                               reinterpret_cast<void*>(this) ) );

  if( static_cast<bool>(core2pin)) {
    cpu_set_t CPUSet;
    CPU_ZERO(&CPUSet);
    CPU_SET(static_cast<int>(core2pin), &CPUSet);

    PTHREAD_CHECK(pthread_setaffinity_np(_pthread_handle,
                                         sizeof(cpu_set_t),
                                         &CPUSet));
  }
}

Thread::~Thread()
{
  _shutdown = true;
  pthread_mutex_lock(&_workMutexStart);
  _workFlag = true;
  pthread_cond_signal(&_workCondStart);
  pthread_mutex_unlock(&_workMutexStart);

  pthread_join(_pthread_handle, NULL);
  _shutdown = false;

  pthread_mutex_destroy(&_workMutexStart);
  pthread_mutex_destroy(&_workMutexEnd);
  pthread_cond_destroy (&_workCondStart);
  pthread_cond_destroy (&_workCondEnd);
}

void
Thread
  ::execute
   ( funcptr_t function
   , void      *arguments )
{
  // Will return immediately if the thread is
  // currently free, i.e. not working.
  wait();

  PTHREAD_CHECK(pthread_mutex_lock(&_workMutexStart));
  _workFlag     = true;
  _workFunction = function;
  _workArgument = arguments;
  PTHREAD_CHECK(pthread_cond_signal(&_workCondStart));
  PTHREAD_CHECK(pthread_mutex_unlock(&_workMutexStart));
}

 void
 Thread
   ::wait()
  {
    pthread_mutex_lock(&_workMutexEnd);
    while(_workFlag) {
      pthread_cond_wait( &_workCondEnd
                       , &_workMutexEnd );
    }
    bool const didThrow ( _didThrow );
    std::string const exceptionWhat ( _workExceptionWhat );
    // Reset the exception flag.
    _didThrow = false;
    // Reset the exception what string
    _workExceptionWhat = std::string();
    pthread_mutex_unlock(&_workMutexEnd);

    if(didThrow) {
      throw UnhandledThreadException(_id,exceptionWhat);
    }
  }

std::set<int>
Thread
  ::active_coreset() {

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  PTHREAD_CHECK(pthread_getaffinity_np(pthread_self(),
                                       sizeof(cpu_set_t),
                                       &cpuset));

  std::set<int> activeset;

  for ( int cpuid(0)
      ;      cpuid < CPU_SETSIZE
      ;    ++cpuid) {
    if (CPU_ISSET(cpuid, &cpuset)) {
      activeset.insert(cpuid);
    }
  }

  return activeset;
}

void *
Thread
  ::main(void * arg)
{
  Thread * const thread(reinterpret_cast<Thread*>(arg));

  bool didThrow; std::string exceptionWhat;
  while(true) {
    pthread_mutex_lock(&thread->_workMutexStart);
    while(!thread->_workFlag) {
      pthread_cond_wait( &thread->_workCondStart
                       , &thread->_workMutexStart);
    }
    pthread_mutex_unlock(&thread->_workMutexStart);

    if(thread->_shutdown) {
      pthread_exit(NULL);
    }

    // Perform the actual work...
    try {
      thread->_workFunction( thread->_id
                           , thread->_workArgument);
      didThrow = false;
    }
    catch(std::exception & e) {
      exceptionWhat = e.what();
      didThrow = true;
    }
    catch(...) {
      didThrow = true;
    }

    pthread_mutex_lock(&thread->_workMutexEnd);
    thread->_workFlag = false;
    thread->_workExceptionWhat = exceptionWhat;
    thread->_didThrow  = didThrow;
    pthread_cond_signal(&thread->_workCondEnd);
    pthread_mutex_unlock(&thread->_workMutexEnd);
  }
}

} /* namespace scheduler */
