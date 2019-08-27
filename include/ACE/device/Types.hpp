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
 * Types.hpp
 *
 */

#ifndef DEVICE_TYPES_HPP_
#define DEVICE_TYPES_HPP_

namespace ace {
namespace device {

/*supported device types*/
enum Type {
  CPU,
  NUMA,
  GPU,
  FPGA,
};

/*device Ids*/
typedef unsigned char Id;

}
}

#endif /* DEVICE_TYPES_HPP_ */