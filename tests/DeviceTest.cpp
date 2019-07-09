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
 * MemoryResourceTest.cpp
 *
 */

#include <gtest/gtest.h>

#include <ACE/device/numa/Device.hpp>
#include <ACE/schedule/Executor.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/task/Task.hpp>

namespace ace {
namespace device {

namespace detail {

class PreCondition
    : public ace::task::PreCondition<int> {

  public:

    PreCondition
      ( int const & lState
      , int const & rState )
    : _lState(lState)
    , _rState(rState)
    {}

    virtual
    ~PreCondition() {}

    virtual bool
    check
      ( int const & state
      , int const & /*first*/
      , int const & /*final*/ ) const {

      return ( ( state <= _lState ) &&
               ( state <= _rState ) );
    }

  private:

    int const & _lState;
    int const & _rState;

};

class PostCondition
    : public ace::task::PostCondition<int> {

  public:

    PostCondition
      ( int & state )
    : _state(state)
    {}

    virtual
    ~PostCondition() {}

    virtual void
    set
      ( int const & state
      , int const & /*first*/
      , int const & /*final*/ ) {
      _state = state+1;
    }

  private:

    int & _state;
};

class Executable
    : public ace::task::Executable<int> {

  public:

    Executable
      ( int & var )
    : _var (var)
    {}

    virtual
    ~Executable()
    { }

    virtual void
    execute
      ( int const & /*state*/
      , int const & /*first*/
      , int const & /*final*/ ) {
      _var+=1;
    }

  private:

    int &                  _var;
};

} // namespace detail


class DeviceTest : public ::testing::Test
{
  protected:

    using State = int;
    using Task = ace::task::Task<State>;
    using Schedule = ace::schedule::Schedule<State>;

    std::vector<int>   _vars;
    State              _initialState;
    State              _finalState;
    std::vector<Task*> _taskList;
    Schedule           _schedule;

  DeviceTest()
  : _vars(1000,0)
  , _initialState(0)
  , _finalState(10000)
  , _taskList(1000)
  {


    std::size_t nTask(_taskList.size());
    for( std::size_t iTask(0)
       ;             iTask < nTask
       ;           ++iTask ) {
      _taskList[iTask] = new Task(_initialState, _finalState);
      _schedule.insert(_taskList[iTask]);
    }

    for( std::size_t iTask(0)
       ;             iTask < nTask
       ;           ++iTask ) {
      int const iLeftTask ( (iTask + nTask - 1) % nTask );
      int const iRightTask( (iTask + nTask + 1) % nTask );

      _taskList[iTask]->insert( std::move( std::unique_ptr<ace::task::PreCondition<int>>(new detail::PreCondition
                                ( _taskList[iLeftTask]->state()
                                , _taskList[iRightTask]->state() ) ) ) );
      _taskList[iTask]->insert(std::move ( std::unique_ptr<ace::task::Executable<int>>(new detail::Executable(_vars[iTask]) ) ) );
      _taskList[iTask]->insert(std::move ( std::unique_ptr<ace::task::PostCondition<int>>(new detail::PostCondition
                                ( _taskList[iTask]->state() ) ) ) );
    }
  }

  ~DeviceTest()
  { }

};

TEST_F(DeviceTest, run)
{
  numa::Device device(0);

  device.scheduler<State>().execute(_schedule);

  for(std::size_t iVar(0)
     ;            iVar < _vars.size()
     ;          ++iVar )
  {
    EXPECT_EQ
      ( _finalState
      , _vars[iVar] );
  }
}

}
}
