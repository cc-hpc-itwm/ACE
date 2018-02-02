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
 * Timer.hpp
 *
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <ctime>
#include <iostream>
#include <x86intrin.h>

namespace ace {

  class Timer {

  public:

    Timer()
    : _tag()
    , _is_running(false)
    , _elapsed_cycles(0.)
    {}

    Timer(std::string const & tag)
    : _tag(tag)
    , _is_running(false)
    , _elapsed_cycles(0.)
    {}

    inline void
    start() {

//      if(_is_running)
//        throw std::logic_error("Timer is already running. Cannot start timer.");
//
//      _is_running = true;
//      clock_gettime(CLOCK_MONOTONIC, &_start);
//      _elapsed_cycles -= static_cast<double>(__rdtsc());
      _elapsed_cycles -= __rdtsc();
    }

    inline void
    stop() {
//      if(!_is_running)
//        throw std::logic_error("Timer is not running. Cannot stop timer.");
//
//      _is_running = false;
//      clock_gettime(CLOCK_MONOTONIC, &_end);
//
//      _elapsed_sec += timeDiff(_end,_start);
//      _elapsed_cycles += static_cast<double>(__rdtsc());
      _elapsed_cycles += __rdtsc();
    }

    double elapsedCycles () {
      return _elapsed_cycles;
    }

//    double elapsedTime () {
//      return _elapsed
//    }

    void print() {

      std::cout << _tag << " timer: "
                 << _elapsed_cycles
                 << " s"
                 << std::endl;
    }


  private:

    std::string const _tag;
    bool _is_running;
//    double _elapsed_sec;
    long _elapsed_cycles;

    struct timespec _start;
    struct timespec _end;

    static double
    timeDiff(struct timespec &end, struct timespec &start) {

      double dt (0.0);
      struct timespec tmp;

      if(end.tv_nsec - start.tv_nsec < 0)
      {
        tmp.tv_sec  = end.tv_sec - start.tv_sec - 1;
        tmp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
      }
      else
      {
        tmp.tv_sec  = end.tv_sec - start.tv_sec;
        tmp.tv_nsec = end.tv_nsec - start.tv_nsec;
      }
      dt = double(tmp.tv_sec);
      dt += 1.0e-9*double(tmp.tv_nsec);

      return dt;
    }

  };

} // namespace ace

#endif /* TIMER_HPP_ */
