/** \file
 * \author Daniel Grünewald, ITWM
 * \date 2013-06
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_TIMERS_H
#define SCHEDULER_TIMERS_H 1

#include <string>


namespace scheduler {

struct Timers {
    public:
      enum Type {TOTAL = 0, SPINLOCK};

      static const std::string descriptions[];

      static unsigned int nr_timers()
      {
        return 2;
      }
};

}   // namespace elastic


#endif    // SCHEDULER_TIMERS_H
