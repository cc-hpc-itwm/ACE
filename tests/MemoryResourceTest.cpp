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

#include <ACE/device/numa/MemoryResource.hpp>
#include <ACE/device/opencl/MemoryResource.hpp>
#include <CL/cl.hpp>

namespace ace {
namespace device {


class MemoryResourceTest : public ::testing::Test
{
  protected:

  std::size_t _segmentSize;


  MemoryResourceTest()
  : _segmentSize(1024*1024)
  { }

  ~MemoryResourceTest()
  { }

};

TEST_F(MemoryResourceTest, allocate)
{
  std::unique_ptr<MemoryResource>  pResource
    (new numa::MemoryResource(0));

  void * palloc( pResource->allocate(10) );

  pResource->deallocate(palloc,10);
}

TEST_F(MemoryResourceTest, equal)
{
  std::unique_ptr<MemoryResource>  pResource1
      (new numa::MemoryResource(0));

  std::unique_ptr<MemoryResource>  pResource2
      (new numa::MemoryResource(0));

  EXPECT_EQ(*pResource1, *pResource2);
}

TEST_F(MemoryResourceTest, openclallocate)
{

  cl::Platform platform(cl::Platform::getDefault());

  std::vector<cl::Device> all_devices;

  platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);

  if(all_devices.size()==0){
    std::stringstream ss;

    ss << "No GPU device found!";

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

}
}
