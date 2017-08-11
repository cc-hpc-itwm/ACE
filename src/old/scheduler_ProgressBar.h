/** \file
 * \author Daniel Grünewald
 * \date 2013-02
 * \copyright Fraunhofer ITWM
 */
#ifndef SCHEDULER_PROGRESSBAR_H
#define SCHEDULER_PROGRESSBAR_H 1

#include "utils/progressbar.h"

#include "scheduler/scheduler_Executable.h"

namespace scheduler {

class ProgressBar : public Executable {
/// \todo create more facilities: percentage, estimated time, don't log via iostream, ...

    public:
      ProgressBar( int nStep )
      : ProgressBar_( nStep )
      , iStep_( 0 )
      , nStep_( nStep )
      {  }

      virtual void
      execute(int timestamp, TimeDirection::Type /* time_direction */)
      {
    	  ++iStep_;

    	  if(  (!(timestamp % (nStep_ / 25 + 1)))  )
    	  {
    		  ProgressBar_.tics_up_to( iStep_ );
    	  }
    	  else if ( iStep_ == nStep_ )
    	  {
    		  ProgressBar_.finished();
    		  ProgressBar_.flush();
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

    private:

      ::ProgressBar ProgressBar_;
      int 		    iStep_;
      int 		    nStep_;

};


}   // namespace scheduler


#endif    // SCHEDULER_PROGRESSBAR_H
