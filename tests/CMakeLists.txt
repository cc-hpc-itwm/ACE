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

add_executable ( ACETests ${sources})

target_include_directories (ACETests PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR} )	

target_link_libraries( ACETests LINK_PUBLIC  
  	GTest::GTest 
	GTest::Main 
	ACE 
	pthread 
	rt )

add_custom_target (run_tests
	COMMAND ACETests
	DEPENDS ACETests
	)
