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
 * Executable.hpp
 *
 */

#ifndef TASK_EXECUTABLE_H
#define TASK_EXECUTABLE_H 1

#include <memory>
#include <vector>

namespace ace {
namespace task {

template <typename State>
class Executable {

public:

  virtual
  ~Executable() {
  }

  virtual void
  execute
    ( State const & ) = 0;

};


template <typename State>
class ExecutableList
    : public Executable<State> {

  public:

    using ElementType = std::unique_ptr<Executable<State> >;

    ExecutableList
      () {
    }

    ExecutableList
      (ExecutableList const&) = delete;

    ExecutableList&
    operator=
      (ExecutableList const&) = delete;

    void
    insert
      (ElementType executable) {
      executables_.push_back(std::move(executable));
    }

    virtual
    ~ExecutableList
      () {
    }

    virtual void
    execute( State const & state ) {
      for ( auto &i: executables_) {
        i->execute(state);
      }
    }

  private:

    std::vector<ElementType> executables_;
};

} // namespace task
} // namespace ace


#endif    // TASK_EXECUTABLE_H
