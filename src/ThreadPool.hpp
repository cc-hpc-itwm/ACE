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
 * ThreadPool.hpp
 *
 */

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <memory>
#include <thread/Barrier.hpp>
#include <thread/Mutex.hpp>
#include <thread/Thread.hpp>

namespace ace {

  //! Determines how threads are pinned to CPU cores
  enum PinningMode
  {
    //! Threads are not pinned at all.
    PIN_NONE,
    //! Pin thread #i to CPU core #i.
    PIN_1TO1,
    //! Pin thread #i to CPU core #i + numThreads.
    PIN_1TO1_OFFSET,
    //! Pin thread #i to #i of set cores in inherited affinity mask.
    PIN_1TO1_INHERITED,
    //! Pin thread #i to CPU core #2i.
    PIN_EVEN,
    //! Pin thread #i to CPU core #2i + 1.
    PIN_ODD,
    //! Pin threads according to a custom pattern.
    PIN_CUSTOM
  };

  //! When constructed, the thread pool spawns a given number of
  //! threads using \c pthread_create() and keeps them running until
  //! the pool is destructed again.
  //! \note  Thread pools cannot be copied and thus do not provide
  //!        a copy constructor or assignment operator!
  //! \brief Manages a pool of threads.
  class ThreadPool
  {

  private:
    //! The number of threads in the pool
    const int _numThreads;
    //! The thread handles
    std::unique_ptr<Thread> *_threads;
    //! A barrier that is used to synchronize all threads
    //! via the barrier() method
    thread::Barrier _barrier;
    //! A mutex that is used by the beginCritical()
    //! and endCritical() methods
    thread::Mutex _mutex;
    //! Pointer to (generic) memory segment that can be accessed by all
    //! threads of the pool. This memory is required for the following
    //! reasons:
    //!  - Some kernels, like the global reduction kernels, need a shared
    //!    memory location to collect the thread local results (reduction).
    //!  - As we do not use the fork-join pattern, we cannot allocate
    //!    shared variables on the fly before the kernel is called.
    //!    Instead, the shared variables must already be known to the
    //!    threads when they are created in the very beginning.
    //!  - In the dot product example, the shared variable cannot be a
    //!    member of the vector class either. Otherwise, the methods
    //!    for dot products and norms cannot be declared const, i.e.,
    //!    it would be impossible for the user to compute the norm of
    //!    a constant vector.
    //!  - The shared memory has void* type, as it must be accessible
    //!    by vectors with different scalar types without making the
    //!    ThreadPool class a template itself.
    //! \note This memory segment is initialized with zero, and the programmer
    //!       must make sure that it is set to zero again before any thread
    //!       returns from a function that uses this variable internally!
    void *_sharedInitZero;
    //! Shared memory segment (see _sharedInitZero).
    //! \note This memory segment is not initialized. The programmer must make
    //!       sure that the value of this variable is not overwritten by a
    //!       thread if the old value is still required by another thread!
    void *_sharedScratch;
    //! A counter that is used in the beginCritical(bool) method.
    int _updateCounterFirst;
    //! A counter that is used in the endCritical(bool) method.
    int _updateCounterLast;
//    //! A numa local allocator
//    memory::AllocatorAlignmentProxy
//      <char,memory::NumaAllocator> *_allocator;

    //! A thread pool cannot be copied.
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

  public:

    //! Type of the function pointers used by the ThreadPool class
    typedef void (*funcptr_t)(int, void*);

    //! Create a pool with the given number of threads.
    //! \param numThreads Number of threads in the pool
    //! \param pinMode    Specifies the way threads are mapped
    //!                   to CPU cores.
    //! \param pinPattern An array of \c numThreads integers
    //!                   that maps each thread to a specific CPU core.
    //! \param pinPattern This \e must be provided when using \c PIN_CUSTOM mode
    //!                   and is meaningless otherwise.
    ThreadPool(int         numThreads,
               PinningMode pinMode     = PIN_NONE,
               int         *pinPattern = NULL);
    //! Destructor
    ~ThreadPool();
    //! This function can be used like \c pthread_create, but it will assign
    //! the work to an existing thread instead of spawning a new one.
    //! \brief   Make a thread call the given function with the given argument.
    //! \param   threadID  The ID of the thread to call the function
    //! \param   function  The function to be called. The actual call will be
    //!                    <c>function(threadID, arguments)</c>.
    //! \param   arguments The arguments to be passed to the function
    //! \throws  PthreadError Unable to assign work to thread
    //! \warning The request will \em not be queued. If the thread is already
    //!          working, this method will block until the previous work
    //!          has been finished!
    void
    run
      ( int       threadID
      , funcptr_t function
      , void      *arguments );
    //! \brief  Wait for a given thread to finish its work.
    //! \throws UnknownThreadException An exception was raised but not caught
    //!                                within the given thread.
    void
    wait
      (int threadID);
    //! Wait for all threads to finish their work.
    void
    waitAll();
    //! Get the number of threads in this pool.
    int
    numThreads() const;
    //! Synchronize all threads of the pool with a barrier.
    //! \note Blocking local collective!
    void
    barrier();
    //! A critical section is a section that can be executed by only
    //! one thread at a time, e.g., to write to a shared variable.
    //! Example:
    //! \code{.cpp}
    //! pool.beginCritical();
    //! sharedInteger += privateInteger;
    //! pool.endCritical();
    //! \endcode
    //! \note Local collective!
    //! \note  In fact, beginCritical() will lock an internal mutex,
    //!        and endCritical() will release it again. Make sure that
    //!        endCritical() is always called and keep in mind that this
    //!        technique is \e not exception safe!
    //! \brief Begin a critical section in the user code.
    void
    beginCritical();
    //! See beginCritical() for more detail.
    //! \brief Begin a critical section in the user code.
    //! \note       Local collective!
    //! \param[out] isFirstThread Indicates whether or not the calling thread
    //!                          is the first one to enter the critical section.
    // We prefer C-style over returning a bool here to keep the signature
    // similar to beginCritical(void).
    void
    beginCritical(bool & isFirstThread);
    //! See beginCritical() for more detail.
    //! \brief End a critical section in the user code.
    //! \note  Local collective!
    void
    endCritical();
    //! See beginCritical() for more detail.
    //! \brief End a critical section in the user code.
    //! \note       Local collective!
    //! \param[out] isLastThread Indicates whether or not the calling thread
    //!                          is the last one to leave the critical section.
    // We prefer C-style over returning a bool here to keep the signature
    // similar to endCritical(void).
    void
    endCritical(bool &isLastThread);

//    memory::AllocatorAlignmentProxy<char,memory::NumaAllocator> &
//    getPinnedAllocator() const;

//    // Give the kernels for global reduction operations access to
//    // the shared memory segment.
//    template <class S>
//    static S
//    globalReducePlus
//      ( int                  threadID
//      , ThreadPool           &pool
//      , const GASPIInterface &interface
//      , S                    value);
//
//    template <class S>
//    static inline void
//    globalReducePlus
//      ( int                  threadID
//      , ThreadPool           &pool
//      , const GASPIInterface &interface
//      , S * const            localValues
//      , S * const            globalValues
//      , int                  numValues );
//
//    template <class S>
//    static S
//    globalReduceMax
//      ( ThreadPool           &pool
//      , const GASPIInterface &interface
//      , S                    value);

    //! Some (public) counters to be shared among all threads
    volatile size_t counter0;
    volatile size_t counter1;
  };

} // namespace ace

#endif /* THREADPOOL_HPP_ */
