enable_testing()
find_package(GTest REQUIRED)

set (sources
	run_tests.cpp
	DeviceTest.cpp
	MemoryResourceTest.cpp )

if (OpenCL)

  add_definitions (-DUSE_OPENCL)

  set (sources ${sources}
  	OpenClDeviceTest.cpp )

endif (OpenCL)

add_executable ( run_tests ${sources})

target_include_directories (run_tests PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR} )	

target_link_libraries( run_tests LINK_PUBLIC  
  	GTest::GTest 
	GTest::Main 
	ACE 
	pthread 
	rt )

add_test(NAME unittest COMMAND run_tests )
