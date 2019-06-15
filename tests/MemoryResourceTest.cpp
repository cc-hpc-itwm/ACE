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

namespace ace {
namespace device {


class MemoryResourceTest : public ::testing::Test
{
  protected:

  std::size_t _segmentSize;


  MemoryResourceTest()
  : _segmentSize(1024*1024)
  {

  }

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

}
}
