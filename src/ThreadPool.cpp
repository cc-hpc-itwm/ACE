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
 */


// ThreadPool.cpp - implementation of the thread pool class

#include <ThreadPool.hpp>
#include <thread/Thread.hpp>
//#include <implementation/implementationDetails.hpp>
//#include <memory/AllocatorAlignmentProxy.hpp>
//#include <memory/NumaAllocator.hpp>
//#include <memory/SegmentBlockAllocator.hpp>

#include <algorithm>
#include <cstring>   // for std::memset()
#include <new>       // for operator new()
#include <exception>
#include <stdexcept> // for std::runtime_error
#include <sstream>
#include <iostream>
#include <set>
extern "C" {
#include <pthread.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

//! Maximum size of a supported scalar type
const size_t MAX_SCALAR_TYPE_SIZE = 64;
const size_t MAX_SCALAR_TYPE_NUM  = 10;

namespace scheduler
{

  ThreadPool::ThreadPool(int numThreads, PinningMode pinMode, int *pinPattern)
  :
    _numThreads(numThreads)
  , _threads (NULL)
  , _barrier(numThreads)
  , _mutex()
  {
    _threads = new std::unique_ptr<Thread>[_numThreads];

    try {
      std::set<int> coreset;
      for(int i(0);i<_numThreads;++i) {

        Optional<int> core2pin;

        if(pinMode != PIN_NONE) {
          switch(pinMode) {
            case PIN_1TO1: {
              core2pin = i;
              coreset.insert(i);
              break;
            }
            case PIN_1TO1_OFFSET: {
              core2pin = i + _numThreads;
              coreset.insert(i+_numThreads);
              break;
            }
            case PIN_1TO1_INHERITED: {
              std::set<int> activecores( Thread::active_coreset() );
              std::set<int>::iterator iter( activecores.begin() );
              std::advance(iter,(i%activecores.size()));
              core2pin = *iter;
              coreset.insert(*iter);
              break;
            }
            case PIN_EVEN: {
              core2pin = 2*i;
              coreset.insert(2*i);
              break;
            }
            case PIN_ODD: {
              core2pin = 2*i + 1;
              coreset.insert(2*i+1);
              break;
            }
            case PIN_CUSTOM: {
              core2pin = pinPattern[i];
              coreset.insert(pinPattern[i]);
              break;
            }
            case PIN_NONE:
            default:
              // No pinning at all
              break;
          }
        }
        _threads[i] = std::unique_ptr<Thread>(new Thread( i,core2pin));
      }

//      _allocator = new  memory::AllocatorAlignmentProxy
//              <char,memory::NumaAllocator>
//                  ( CACHE_LINE_SIZE
//                  , memory::NumaAllocator<char>
//                      (coreset)
//                  );

      try {

        _sharedInitZero = NULL;
        // Using operator new() allows us to allocate raw memory segments
        // (void*) of a specific size, similar to malloc() in C.
        _sharedInitZero = operator new
            ( numThreads
            * MAX_SCALAR_TYPE_SIZE
            * MAX_SCALAR_TYPE_NUM );

        try {
          _sharedScratch  = NULL;
          _sharedScratch  = operator new
              ( numThreads
              * MAX_SCALAR_TYPE_SIZE
              * MAX_SCALAR_TYPE_NUM );

          try {

            // Initialize shared variable with zero.
            // This is required by some kernels!
            std::memset( _sharedInitZero
                       , 0
                       , numThreads
                       * MAX_SCALAR_TYPE_SIZE
                       * MAX_SCALAR_TYPE_NUM );

            _updateCounterFirst = 0;
            _updateCounterLast  = 0;

          } catch(...) {
            operator delete(_sharedScratch);
            throw;
          }

        } catch(...) {
          operator delete(_sharedInitZero);
          throw;
        }

      } catch (...) {
//        delete _allocator;
        throw;
      }

    } catch(...) {
      delete[] _threads;
      throw;
    }
  }

  ThreadPool::~ThreadPool()
  {
    waitAll();

    delete[] _threads;

    // Use operator delete() here, since operator new() was
    // used to allocate the memory segments.
    operator delete(_sharedInitZero);
    operator delete(_sharedScratch);

//    delete _allocator;
  }

  void ThreadPool::run(int       threadID,
                       funcptr_t function,
                       void      *arguments)
  {
    _threads[threadID]->execute(function,arguments);
  }

  void ThreadPool::wait(int threadID)
  {
    _threads[threadID]->wait();
  }


  void ThreadPool::waitAll()
  {
    for(int i = 0; i < _numThreads; i++) wait(i);
  }

  int ThreadPool::numThreads() const
  {
    return _numThreads;
  }

  void
  ThreadPool
    ::barrier()
  {
    _barrier.apply();
  }

  void
  ThreadPool
    ::beginCritical()
  {
    _mutex.lock();
  }

  void
  ThreadPool
    ::beginCritical(bool &isFirstThread)
  {
    _mutex.lock();
    isFirstThread = static_cast<bool>(!_updateCounterFirst);
    _updateCounterFirst = (_updateCounterFirst + 1)%_numThreads;
  }

  void
  ThreadPool
    ::endCritical()
  {
    _mutex.unlock();
  }

  void
  ThreadPool
    ::endCritical(bool &isLastThread)
  {
    isLastThread = static_cast<bool>(
        !(_updateCounterLast = (_updateCounterLast + 1)%_numThreads)
    );
    _mutex.unlock();
  }

//  memory::AllocatorAlignmentProxy<char,memory::NumaAllocator> &
//  ThreadPool
//    ::getPinnedAllocator() const
//  {
//    return *_allocator;
//  }

  UnhandledThreadException::UnhandledThreadException( int threadID
                                                    , std::string what)
  {
    std::stringstream ss;

    if(what.empty()) {
      ss << "unknown exception thrown by thread #"
         << threadID;
    } else {
      ss << "thread #"
         << threadID
         << ": "
         << what;
    }
    _whatString = ss.str();
  }

  const char* UnhandledThreadException::what() const throw()
  {
    return _whatString.c_str();
  }

} // namespace GaspiLS

