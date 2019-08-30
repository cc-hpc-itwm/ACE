/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2019
 * 
 * This file is part of ACE.
 * 
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 * 
 * ACE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * Scheduler.cpp
 *
 */

#include <ACE/device/numa/Scheduler.hpp>

#include <ACE/utility/Macros.hpp>


namespace ace {
namespace device {
namespace numa {

namespace detail {

int
get_affinity_mask (const int sock, cpu_set_t * cpuset)
{
  int i, rc;
  char buf[1024];
  unsigned int m[256];
  char path[256];

  memset (buf, 0, 1024);
  memset (m, 0, 256 * sizeof (unsigned int));

  snprintf (path, 256, "/sys/devices/system/node/node%d/cpumap", sock);

  FILE *f = fopen (path, "r");
  if( f == NULL )
  {
    std::stringstream ss;

    ss << "Could not open cpumap for node "
       << sock
       << ": "
       << path;

    throw std::runtime_error
      (CODE_ORIGIN + ss.str());
  }

  //read cpumap
  int id = 0;
  if( fgets(buf, sizeof(buf), f) )
  {
    char *bptr = buf;

    while (1)
    {
      int ret = sscanf (bptr, "%x", &m[id]);
      if( ret <= 0 )
      {
          break;
      }

      int found = 0;
      unsigned int cpos = 0;

      size_t length = strlen(bptr);
      size_t j;

      for(j = 0;j < length - 1; j++)
      {
        if( bptr[j]==',' )
        {
          found=1;
          break;
        }
        cpos++;
      }

      if( !found )
      {
        if( (cpos+1) > strlen(bptr) )
        {
          fclose(f);
          throw std::runtime_error(CODE_ORIGIN);
        }
      }

      bptr += (cpos+1);
      id++;
    }
  }

  rc = id;

  memset (cpuset, 0, sizeof (cpu_set_t));

  char *ptr = (char *) cpuset;
  int pos = 0;

  for(i = rc - 1; i >= 0; i--)
  {
    memcpy (ptr + pos, &m[i], sizeof (unsigned int));
    pos += sizeof (unsigned int);
  }

  fclose (f);

  return CPU_COUNT(cpuset);
}

int
set_socket_affinity (const Id sock)
{
  cpu_set_t sock_mask;

  int active_cores ( get_affinity_mask (sock, &sock_mask) );

  if( sched_setaffinity (0, sizeof (cpu_set_t), &sock_mask) != 0 )
  {
    throw std::runtime_error
      (CODE_ORIGIN + "Failed to set affinity");
  }

  return active_cores;
}

}

SchedulerRuntime
  ::SchedulerRuntime
   ( Id const & id )
: _pThreadPool
    ( std::make_shared<thread::Pool>
        ( detail::set_socket_affinity(id),thread::PIN_1TO1_INHERITED) )
{ }

}
}
}
