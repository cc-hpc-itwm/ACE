/** \file
 * \author Leo Nesemann
 * \date 2013-01
 * \copyright Fraunhofer ITWM
 */
#ifndef TASK_EXECUTABLE_H
#define TASK_EXECUTABLE_H 1

#include <memory>
#include <vector>

namespace scheduler {

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

}   // namespace scheduler


#endif    // TASK_EXECUTABLE_H
