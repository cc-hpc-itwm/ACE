#include <schedule/Executor.hpp>
#include <schedule/Schedule.hpp>
#include <task/Task.hpp>
#include <iostream>
#include <sstream>

namespace detail {

class PreCondition
    : public ace::task::PreCondition<int> {

  public:

    PreCondition
      ( std::string const & name
      , int const & lState
      , int const & rState )
    : _name(name)
    , _lState(lState)
    , _rState(rState)
    {}

    virtual
    ~PreCondition() {}

    virtual bool
    check(int const & state) const {

//      std::stringstream ss;
//
//      ss << "pre-cond check of " << _name << ": ";
//      ss << state;
//      ss << " -> (" << _lState << "," << _rState << ")";
//
//      std::cout << ss.str() << std::endl;

      return ( ( state <= _lState ) &&
               ( state <= _rState ) );
    }

  private:

    std::string const _name;
    int const & _lState;
    int const & _rState;

};

class PostCondition
    : public ace::task::PostCondition<int> {

  public:

    PostCondition
      ( std::string const & name
      , int & state )
    : _name(name)
    , _state(state)
    {}

    virtual
    ~PostCondition() {}

    virtual void
    set(int const & state) {
//       std::stringstream ss;
//
//       ss << "post-cond set of " << _name << ": ";
//       ss << _state;
      _state = state+1;
//       ++_state;
//       ss << " -> " << _state;
//       std::cout << ss.str() << std::endl;
    }

  private:

    std::string const _name;
    int & _state;
};

class Executable
    : public ace::task::Executable<int> {

  public:

    Executable
      (std::string const & name)
    : _name(name)
    , _pVar( new int [16] )
    , _var (_pVar[0])
    , _time()
    {}

    virtual
    ~Executable() {
      std::cout << "final result of "
                      << _name
                      << ": "
                      << _var
                      << ", time = "
                      << _time.elapsedTime()
                      << std::endl;
    }

    virtual void
    execute(int const & /*state*/) {
//      std::cout << "execute of "
//                << _name
//                << ": "
//                << state
//                << std::endl;
      _time.start();
      _var+=1;
      _time.stop();
    }

  private:

    std::string const _name;

    std::unique_ptr<int[]> _pVar;
    int &                  _var;
    ace::Timer       _time;

};

class ExecutableDirect {

  public:

    ExecutableDirect
      (std::string const & name)
    : _name(name)
    , _var (0)
    , _time()
    {}

    virtual
    ~ExecutableDirect() {
      std::cout << "final result of "
                      << _name
                      << ": "
                      << _var
                      << ", time = "
                      << _time.elapsedTime()
                      << std::endl;
    }

    inline void
    execute(int const & /*state*/) {
//      std::cout << "execute of "
//                << _name
//                << ": "
//                << state
//                << std::endl;
      _time.start();
      _var+=1;
      _time.stop();
    }

  private:

    std::string const _name;

    int _var;
    ace::Timer _time;

};

}

int
main
  ( int /*argc*/
  , char */*argv*/[] ) {

  using State = int;
  using Task = ace::task::Task<State>;
  using Schedule = ace::schedule::Schedule<State>;
  using Executer = ace::schedule::ScheduleExecuter<State>;
  using ThreadPool = ace::thread::Pool;

  ThreadPool threadPool(2);

  State initialState(0);
  State finalState(10000);

  int const nTask(20);

  Schedule schedule;
  {
    std::vector<Task*> taskList(nTask);

    for( int iTask(0)
       ;     iTask < nTask
       ;   ++iTask ) {
      taskList[iTask] = new Task(initialState, finalState);
      schedule.insert(taskList[iTask]);
    }

    for( int iTask(0)
       ;     iTask < nTask
       ;   ++iTask ) {
      int const iLeftTask ( (iTask + nTask - 1) % nTask );
      int const iRightTask( (iTask + nTask + 1) % nTask );

      std::stringstream ss; ss << iTask;

      taskList[iTask]->insert( std::move( std::unique_ptr<ace::task::PreCondition<int>>(new detail::PreCondition
                                ( ss.str()
                                , taskList[iLeftTask]->state()
                                , taskList[iRightTask]->state() ) ) ) );
      taskList[iTask]->insert(std::move ( std::unique_ptr<ace::task::Executable<int>>(new detail::Executable
                                ( ss.str() ) ) ) );
      taskList[iTask]->insert(std::move ( std::unique_ptr<ace::task::PostCondition<int>>(new detail::PostCondition
                                ( ss.str()
                                , taskList[iTask]->state() ) ) ) );
    }

//    {
//      bool check(left->pre_condition().check(left->state()));
//      std::cout << (check?"yes":"no") << std::endl;
//    }
//
//    {
//      bool check(middle->pre_condition().check(middle->state()));
//      std::cout << (check?"yes":"no") << std::endl;
//    }
//
//    {
//      bool check(right->pre_condition().check(right->state()));
//      std::cout << (check?"yes":"no") << std::endl;
//    }
  }


  Executer( schedule
          , threadPool ).execute();

  {
    ace::Timer singleTimer;

    double val(0.);

    singleTimer.start();
    for(int iTask(0);iTask<nTask;++iTask) {
      for(int iState(initialState);iState<finalState;++iState) {
        val +=1;
      }
    }
    singleTimer.stop();

    std::cout << "total run time : " << singleTimer.elapsedTime() << std::endl;
  }

  {
    ace::Timer singleTimer;

    int const testState(0);

    singleTimer.start();
    for(int iTask(0);iTask<nTask;++iTask) {

      detail::Executable executable("virtual executable");

      for(int iState(initialState);iState<finalState;++iState) {
        executable.execute(testState);
      }
    }
    singleTimer.stop();

    std::cout << "total run time direct virtual: " << singleTimer.elapsedTime() << std::endl;
  }

  {
    ace::Timer singleTimer;

    int const testState(0);

    singleTimer.start();
    for(int iTask(0);iTask<nTask;++iTask) {

      detail::Executable executable("virtual base executable");

      ace::task::Executable<int> & baseExecutable(executable);

      for(int iState(initialState);iState<finalState;++iState) {
        baseExecutable.execute(testState);
      }
    }
    singleTimer.stop();

    std::cout << "total run time direct virtual base: " << singleTimer.elapsedTime() << std::endl;
  }

  {
    ace::Timer singleTimer;

    int const testState(0);

    singleTimer.start();
    for(int iTask(0);iTask<nTask;++iTask) {

      detail::ExecutableDirect executable("direct executable");

      for(int iState(initialState);iState<finalState;++iState) {
        executable.execute(testState);
      }
    }
    singleTimer.stop();

    std::cout << "total run time direct: " << singleTimer.elapsedTime() << std::endl;
  }

}
