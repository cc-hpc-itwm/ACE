/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2017
 *
 * This file is part of ACE.
 *
 * ACE is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 *
 * GaspiCxx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ACE. If not, see <http://www.gnu.org/licenses/>.
 *
 * Macros.hpp
 *
 */

#ifndef ACE_MACROS_HPP_
#define ACE_MACROS_HPP_

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace ace {

namespace macro_detail {

struct CodeOriginString : public std::string
{
  CodeOriginString (const char* s, const int i) : std::string ()
  {
    std::ostringstream oss;
    oss << s << " [" << i << "]: ";

    assign (oss.str());
  }
};

}

// macro definitions

#define CODE_ORIGIN                             \
  macro_detail::CodeOriginString                \
    (__PRETTY_FUNCTION__, __LINE__)             \

}

#endif /* ACE_MACROS_HPP_ */
