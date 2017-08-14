#include <scheduler_Schedule.h>
#include <scheduler_Task.h>
#include <scheduler_TaskDef.h>
#include <ScheduleExecuter.h>
#include <iostream>
#include <sstream>

namespace detail {

class PreCondition
    : public scheduler::PreCondition<int> {

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

      std::stringstream ss;

      ss << "pre-cond check of " << _name << ": ";
      ss << state;
      ss << " -> (" << _lState << "," << _rState << ")";

      std::cout << ss.str() << std::endl;

      return ( ( state <= _lState ) &&
               ( state <= _rState ) );
    }

  private:

    std::string const _name;
    int const & _lState;
    int const & _rState;

};

class PostCondition
    : public scheduler::PostCondition<int> {

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
       std::stringstream ss;

       ss << "post-cond set of " << _name << ": ";
       ss << _state;
      _state = state+1;
       ss << " -> " << _state;
       std::cout << ss.str() << std::endl;
    }

  private:

    std::string const _name;
    int & _state;
};

class Executable
    : public scheduler::Executable<int> {

  public:

    Executable
      (std::string const & name)
    : _name(name)
    , _var (0)
    {}

    virtual
    ~Executable() {
      std::cout << "final result of "
                      << _name
                      << ": "
                      << _var
                      << std::endl;
    }

    virtual void
    execute(int const & state) {
      std::cout << "execute of "
                << _name
                << ": "
                << state
                << std::endl;
      _var+=1;
    }

  private:

    std::string const _name;

    int _var;

};

}

int
main
  ( int /*argc*/
  , char */*argv*/[] ) {

  using State = int;
  using Task = scheduler::Task<State>;
  using Schedule = scheduler::Schedule<State>;
  using Executer = scheduler::ScheduleExecuter<State>;
  using ThreadPool = scheduler::ThreadPool;

  ThreadPool threadPool(10);

  Schedule schedule;
  {
    State initialState(0);
    State finalState(10);

    int const nTask(10);
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

      taskList[iTask]->insert(new detail::PreCondition
                                ( ss.str()
                                , taskList[iLeftTask]->state()
                                , taskList[iRightTask]->state() ) );
      taskList[iTask]->insert(new detail::Executable
                                ( ss.str() ) );
      taskList[iTask]->insert(new detail::PostCondition
                                ( ss.str()
                                , taskList[iTask]->state() ) );
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

}
