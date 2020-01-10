/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2019
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
 * PinningMode.hpp
 *
 */

#ifndef THREAD_PINNING_MODE_HPP_
#define THREAD_PINNING_MODE_HPP_

namespace ace {
namespace thread {

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

} // namespace thread
} // namespace ace

#endif /* THREADPOOL_HPP_ */
