/** \file */
#ifndef LOCK_H
#define LOCK_H 1

//#include "MCTP2.h"
#include <stdexcept>


/** \brief A RAII locking class
 *
 * This class implements a RAII lock with the MCTP locking mechanism (see e.g.
 * Meyers, Effective C++, 3rd ed., item 14). Locks may have different IDs of
 * type unsigned char.
 * \note To keep lock IDs unique, the different lock types are defined already here,
 *       and not in the respective classes.
 */
class Lock
{
    public:
      Lock(unsigned char id)
        : id_(id)
      {
//        mctp2Lock(id_);
        throw std::runtime_error("not implemented yet");
      }

      virtual ~Lock()
      {
//        mctp2Unlock(id_);
      }

      static unsigned char ScheduleModifications()
      {
        return 0;
      }

      static unsigned char DmaQueueOperations()
      {
        return 1;
      }

      static unsigned char BufferCounters()
      {
        return 2;
      }

      static unsigned char Logging()
      {
        return 3;
      }

      static unsigned char PinThread()
      {
        return 4;
      }

      static unsigned char Papi()
      {
	return 5;
      }


    private:
      Lock(Lock const&);
      Lock& operator=(Lock const&);

      unsigned char const id_;
};


#endif    // LOCK_H
