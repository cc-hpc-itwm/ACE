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
#include <ACE/device/opencl/MemoryResource.hpp>
#include <ACE/device/opencl/Kernel.hpp>
#include <ACE/device/Types.hpp>
#include <ACE/schedule/Executor.hpp>
#include <ACE/schedule/Schedule.hpp>
#include <ACE/task/Task.hpp>

#include <CL/cl.hpp>

#include <vector>


namespace {

  template <typename GI>
  GI div_roundup(GI numerator, GI denominator)
  { return ( numerator + denominator - static_cast<GI>(1) ) / denominator; }

  template <class GI>
  class UniformPartition
  {

  public:

    UniformPartition( GI begin, GI end, unsigned short nparts )
    : _begin(begin)
    , _nelems(end-begin)
    , _nparts(nparts)
    {}

    GI
    begin( unsigned short ipart) const
    { return _begin
          + div_roundup( static_cast<GI>(ipart) * _nelems, _nparts ); }

    GI
    end( unsigned short ipart ) const
    { return begin(ipart + 1); }

    GI
    nparts() const
    { return _nparts; }

    GI
    size(unsigned short ipart ) const
    { return end(ipart) - begin(ipart); }

    GI
    size() const
    { return _nelems; }

  private:

    GI _begin;
    GI _nelems;
    GI _nparts;

  };

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
      ( cl::Kernel & kernel_e
      , cl::Kernel & kernel_o
      , cl::CommandQueue & queue )
    : _kernel_e (kernel_e)
    , _kernel_o (kernel_o)
    , _queue (queue)
    { }

    virtual
    ~Executable()
    { }

    void
    execute
      ( int const & state
      , int const & first
      , int const & /*final*/ ) override {

      if((state-first)%2 == 0 ) {
        _queue.enqueueTask(_kernel_e);
      }
      else {
        _queue.enqueueTask(_kernel_o);
      }
    }

    ace::device::Kernel &
    getKernel
      ( int const & state
      , int const & first
      , int const & /*final*/ ) override {
      if((state-first)%2 == 0 ) {
        return _kernel_e;
      }
      else {
        return _kernel_o;
      }
    }

  private:

    ace::device::opencl::Kernel _kernel_e;
    ace::device::opencl::Kernel _kernel_o;
    cl::CommandQueue & _queue;
};

} // anonymous namespace

namespace ace {
namespace device {

class OpenClDeviceTest : public ::testing::Test
{
  protected:

    using State = int;
    using Task = ace::task::Task<State>;
    using Schedule = ace::schedule::Schedule<State>;



  OpenClDeviceTest()
  { }

  ~OpenClDeviceTest()
  { }

};

TEST_F(OpenClDeviceTest, openclallocate)
{

  cl::Platform platform(cl::Platform::getDefault());

  std::vector<cl::Device> all_devices;

  platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

  if(all_devices.size()==0){
    std::stringstream ss;

    ss << "No OpenCL device found!";

    throw std::runtime_error
      (ss.str());
  }

 cl::Device device(all_devices[0]);

 cl::Context context(device);

 cl::CommandQueue queue(context, device);

  std::unique_ptr<opencl::MemoryResource>  pResource
    (new opencl::MemoryResource(context,queue));

  std::size_t nByte(10);

  void * palloc1( pResource->allocate(nByte) );
  void * palloc2( pResource->allocate(nByte) );
  void * palloc3( pResource->allocate(nByte) );

  std::cout << *pResource << std::endl;

  for(std::size_t iByte(0);iByte<nByte;++iByte) {
    void * palloc4 (static_cast<uint8_t*>(palloc2) + iByte);

    opencl::MemoryResource::DeviceBuffer::Pointer pDevice;
    std::size_t pOffset(0);

    pResource->getOpenCLBufferAndOffset
        ( palloc4
        , pDevice
        , pOffset );

    EXPECT_EQ(pOffset,iByte);
  }


  pResource->deallocate(palloc3,nByte);
  pResource->deallocate(palloc2,nByte);
  pResource->deallocate(palloc1,nByte);
}

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
    c[i] = n;
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

TEST_F(OpenClDeviceTest, Iterative)
{

  opencl::Device gpu_device(GPU,0);

  int const n(101);
  std::vector<int,Allocator<int>> a(n,gpu_device.allocator<int>());
  std::vector<int,Allocator<int>> b(n,gpu_device.allocator<int>());

  for(int i(0);i<n;++i) {
    a[i] = 0;
    b[i] = 1;
  }

  gpu_device.updateDevice<int>(b.data(),b.size());

  // calculates for each element; next = ( curr(left) + curr(right) ) / 2 +1
  std::string kernel_code =
    "   void kernel simple_stencil_e(global int* A, global const int* B,"
    "                                const int N) {"
    "       int ID, Nthreads, n, ratio, start, stop, il, ir;"
    ""
    "       ID = get_global_id(0);"
    "       Nthreads = get_global_size(0);"
    "       n = N;"
    ""
    "       ratio = (n / Nthreads);"  // number of elements for each thread
    "       start = ratio * ID;"
    "       stop  = ratio * (ID + 1);"
    ""
    "       for (int i=start; i<stop; i++) {"
    "         il = ( i + n - 1 ) % n;"
    "         ir = ( i + n + 1 ) % n;"
    "         A[i] = (B[il] + B[ir]) / 2 + 1;"
    "       }"
    "   }"
    ""
    "   void kernel simple_stencil_o(global const int* A, global int* B,"
    "                                const int N) {"
    "       int ID, Nthreads, n, ratio, start, stop, il, ir;"
    ""
    "       ID = get_global_id(0);"
    "       Nthreads = get_global_size(0);"
    "       n = N;"
    ""
    "       ratio = (n / Nthreads);"  // number of elements for each thread
    "       start = ratio * ID;"
    "       stop  = ratio * (ID + 1);"
    ""
    "       for (int i=start; i<stop; i++) {"
    "         il = ( i + n - 1 ) % n;"
    "         ir = ( i + n + 1 ) % n;"
    "         B[i] = (A[il] + A[ir]) / 2 + 1;"
    "       }"
    "   }";

  cl::Kernel kernel_e
    (gpu_device.getBoundKernel
       ( kernel_code
       ,"simple_stencil_e"
       , a.data()
       , b.data()
       , n
       )
    );

  cl::Kernel kernel_o
    (gpu_device.getBoundKernel
       ( kernel_code
       ,"simple_stencil_o"
       , a.data()
       , b.data()
       , n
       )
    );

  for(int it(0);it<50;++it) {
    // execute the kernel on GPU
    gpu_device.getQueue().enqueueTask(kernel_e);
    gpu_device.getQueue().enqueueTask(kernel_o);
  }

  // read result from GPU to here
  gpu_device.updateHost<int>(a.data(),a.size());


  for (int i=0; i<n; i++) {
    EXPECT_EQ( a[i], 100 );
  }
}

TEST_F(OpenClDeviceTest, Task)
{

  opencl::Device gpu_device(GPU,0);

  int const n(101);
  std::vector<int,Allocator<int>> a(n,gpu_device.allocator<int>());
  std::vector<int,Allocator<int>> b(n,gpu_device.allocator<int>());

  for(int i(0);i<n;++i) {
    a[i] = 0;
    b[i] = 1;
  }

  gpu_device.updateDevice<int>(b.data(),b.size());

  // calculates for each element; next = ( curr(left) + curr(right) ) / 2 +1
  std::string kernel_code =
    "   void kernel simple_stencil_e"
    "           ( global int* A, global int const* B,"
    "             const int N, const int iBeg, const int iEnd) {"
    "       int ID, Nthreads, n, ratio, start, stop, il, ir;"
    ""
    "       ID = get_global_id(0);"
    "       Nthreads = get_global_size(0);"
    "       n = N;"
    ""
    "       ratio = ( (iEnd-iBeg) / Nthreads);"  // number of elements for each thread
    "       start = iBeg + ratio * (ID + 0);"
    "       stop  = iBeg + ratio * (ID + 1);"
    ""
    "       for (int i=start; i<stop; i++) {"
    "         il = ( i + n - 1 ) % n;"
    "         ir = ( i + n + 1 ) % n;"
    "         A[i] = (B[il] + B[ir]) / 2 + 1;"
    "       }"
    "   }"
    ""
    "   void kernel simple_stencil_o"
    "           ( global int const * A, global int * B,"
    "             const int N, const int iBeg, const int iEnd) {"
    "       int ID, Nthreads, n, ratio, start, stop, il, ir;"
    ""
    "       ID = get_global_id(0);"
    "       Nthreads = get_global_size(0);"
    "       n = N;"
    ""
    "       ratio = ( (iEnd-iBeg) / Nthreads);"  // number of elements for each thread
    "       start = iBeg + ratio * (ID + 0);"
    "       stop  = iBeg + ratio * (ID + 1);"
    ""
    "       for (int i=start; i<stop; i++) {"
    "         il = ( i + n - 1 ) % n;"
    "         ir = ( i + n + 1 ) % n;"
    "         B[i] = (A[il] + A[ir]) / 2 + 1;"
    "       }"
    "   }";

  {
    int const nPart(11);

    ::UniformPartition<int> part(0,n,nPart);

    State              initialState(0);
    State              finalState  (100);
    std::vector<Task*> taskList    (nPart);
    Schedule           schedule;

    std::size_t nTask(taskList.size());
    for( std::size_t iTask(0)
       ;             iTask < nTask
       ;           ++iTask ) {
      taskList[iTask] = new Task(initialState, finalState);
      schedule.insert(taskList[iTask]);
    }

    for( std::size_t iTask(0)
       ;             iTask < nTask
       ;           ++iTask ) {

      int const iLeftTask ( (iTask + nTask - 1) % nTask );
      int const iRightTask( (iTask + nTask + 1) % nTask );

      taskList[iTask]->insert
        ( std::move
          ( std::unique_ptr<ace::task::PreCondition<int>>
            ( new ::PreCondition
                ( taskList[iLeftTask]->state()
                , taskList[iRightTask]->state() ) ) ) );

      cl::Kernel kernel_e
          (gpu_device.getBoundKernel
             ( kernel_code
             ,"simple_stencil_e"
             , a.data()
             , b.data()
             , part.size()
             , part.begin(iTask)
             , part.end(iTask)
             )
          );

      cl::Kernel kernel_o
        (gpu_device.getBoundKernel
           ( kernel_code
           ,"simple_stencil_o"
           , a.data()
           , b.data()
           , part.size()
           , part.begin(iTask)
           , part.end(iTask)
           )
        );

      taskList[iTask]->insert
        ( std::move
          ( std::unique_ptr<ace::task::Executable<int>>
            ( new ::Executable
                ( kernel_e
                , kernel_o
                , gpu_device.getQueue() ) ) ) );

      taskList[iTask]->insert
        (std::move
         ( std::unique_ptr<ace::task::PostCondition<int>>
           ( new ::PostCondition
                ( taskList[iTask]->state() ) ) ) );
    }

    gpu_device.scheduler<State>().execute(schedule);

  }

  // read result from GPU to here
  gpu_device.updateHost<int>(a.data(),a.size());

  for (int i=0; i<n; i++) {
    EXPECT_EQ( a[i], 100 );
  }

}

}
}
