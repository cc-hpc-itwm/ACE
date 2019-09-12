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
#include <ACE/device/opencl/Device.hpp>
#include <ACE/schedule/Executor.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/task/Task.hpp>

#include <vector>

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


class OpenClDeviceTest : public ::testing::Test
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

  OpenClDeviceTest()
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

  ~OpenClDeviceTest()
  { }

};

TEST_F(OpenClDeviceTest, run)
{
  opencl::Device gpu_device(GPU,0);

  int & n(*gpu_device.allocator<int>().allocate(1)); n=101;
  std::vector<int,Allocator<int>> a(n,gpu_device.allocator<int>());
  std::vector<int,Allocator<int>> b(n,gpu_device.allocator<int>());
  std::vector<int,Allocator<int>> c(n,gpu_device.allocator<int>());

  for(int i(0);i<n;++i) {
    a[i] = i;
    b[i] = n -i - 1;
  }

  gpu_device.updateDevice<int>(a.data(),a.size());
  gpu_device.updateDevice<int>(b.data(),b.size());
  gpu_device.updateDevice<int>(&n,1);

  // calculates for each element; C = A + B
  std::string kernel_code=
      "   void kernel simple_add(global const int* A, global const int* B, global int* C, "
      "                          global const int* N) {"
      "       int ID, Nthreads, n, ratio, start, stop;"
      ""
      "       ID = get_global_id(0);"
      "       Nthreads = get_global_size(0);"
      "       n = N[0];"
      ""
      "       ratio = (n / Nthreads);"  // number of elements for each thread
      "       start = ratio * ID;"
      "       stop  = ratio * (ID + 1);"
      ""
      "       for (int i=start; i<stop; i++)"
      "           C[i] = A[i] + B[i];"
      "   }";

  cl::Kernel kernel
    (gpu_device.getBoundKernel
       ( kernel_code
       ,"simple_add"
       , a.data()
       , b.data()
       , c.data()
       , &n
       )
    );

  // execute the kernel on GPU
  gpu_device.getQueue().enqueueTask(kernel);

  // read result from GPU to here
  gpu_device.updateHost<int>(c.data(),c.size());

  for (int i=0; i<n; i++) {
    EXPECT_EQ( c[i], n-1 );
  }

  gpu_device.allocator<int>().deallocate(&n,1);

}

}
}
