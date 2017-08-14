/*
 * Timer.hpp
 *
 *  Created on: Aug 3, 2016
 *      Author: gruenewa
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <ctime>
#include <iostream>

namespace scheduler {

  class Timer {

  public:

    Timer()
    : _tag()
    , _is_running(false)
    , _elapsed_sec(0.)
    {}

    Timer(std::string const & tag)
    : _tag(tag)
    , _is_running(false)
    , _elapsed_sec(0.)
    {}

    void start() {

      if(_is_running)
        throw std::logic_error("Timer is already running. Cannot start timer.");

      _is_running = true;
      clock_gettime(CLOCK_MONOTONIC, &_start);
    }

    void stop() {
      if(!_is_running)
        throw std::logic_error("Timer is not running. Cannot stop timer.");

      _is_running = false;
      clock_gettime(CLOCK_MONOTONIC, &_end);

      _elapsed_sec += timeDiff(_end,_start);
    }

    double elapsedTime () {
      return _elapsed_sec;
    }

    void print() {

      std::cout << _tag << " timer: "
                 << _elapsed_sec
                 << " s"
                 << std::endl;
    }


  private:

    std::string const _tag;
    bool _is_running;
    double _elapsed_sec;

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

}

#endif /* TIMER_HPP_ */
