/** \file
 * \author Leo Nesemann
 * \date 2013-02
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_TIMESTAMP_LOGGER_H
#define SCHEDULER_TIMESTAMP_LOGGER_H 1

#include "scheduler/scheduler_Executable.h"

#include <iostream>
#include <limits>


namespace scheduler {

class TimestampLogger : public Executable {
/// \todo create more facilities: percentage, estimated time, don't log via iostream, ...

    public:
      TimestampLogger()
      {  }

      virtual void execute(int timestamp, TimeDirection::Type /* time_direction */)
      {
        if (! (timestamp % 10) )
        {
          std::cout << "  " << timestamp << std::endl;
        }
      }

      /** \brief returns an impossible timer ID
       *
       * This implementation returns an impossible timer ID, such that no time measurement is done for the logging
       * (except the total time).
       * \note If logging takes up a significant amount of time, an appropriate ID should be returned,
       *       which must not interfere with the other timer IDs. In that case, a child class in the appropriate
       *       namespace should be created, which can then return an ID depending on elastic::Timers, TTI::Timers
       *       etc.
       */
      virtual unsigned int timer_ID() const
      {
        return std::numeric_limits<unsigned int>::max();
      }

};


}   // namespace scheduler


#endif    // SCHEDULER_TIMESTAMP_LOGGER_H
