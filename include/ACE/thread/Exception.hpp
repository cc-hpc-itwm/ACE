/*
 * Copyright (c) Fraunhofer ITWM - <http://www.itwm.fraunhofer.de/>, 2017
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
 * Exception.hpp
 *
 */

#ifndef THREAD_EXCEPTION_H_
#define THREAD_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace ace {

class UnhandledThreadException
    : public std::exception {

private:

    std::string _whatString;

public:

    UnhandledThreadException(int threadID, std::string what = std::string() );
    virtual ~UnhandledThreadException() throw() {}
    virtual const char* what() const throw();
};

} // namespace ace

#endif /* THREAD_EXCEPTION_H_ */
