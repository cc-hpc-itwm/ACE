cmake_minimum_required (VERSION 2.8.11)
project (ACE)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
set (GTEST_ROOT "/opt/google/test")

add_subdirectory (src)
#add_subdirectory (examples)
if (Test)
#  add_subdirectory (tests)
endif (Test)  
