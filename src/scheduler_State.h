/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_STATE_H
#define SCHEDULER_STATE_H 1

namespace scheduler {

class State
{

  public:

    virtual State &
    operator++(int) const = 0;

    virtual bool
    operator>=
      ( State const & ) const = 0;

};

}   // namespace scheduler


#endif    // SCHEDULER_STATE_H
