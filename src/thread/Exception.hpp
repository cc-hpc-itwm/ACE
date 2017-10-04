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
 * Thread.h
 *
 */

#ifndef THREAD_EXCEPTION_H_
#define THREAD_EXCEPTION_H_

#include <pthread.h>
#include <stdexcept>
#include <string>
#include <utility/Optional.hpp>
#include <set>

namespace scheduler {

class UnhandledThreadException : public std::exception {

private:

    std::string _whatString;

public:

    UnhandledThreadException(int threadID, std::string what = std::string() );
    virtual ~UnhandledThreadException() throw() {}
    virtual const char* what() const throw();
};

} /* namespace scheduler */

#endif /* THREAD_EXCEPTION_H_ */
