# Define library. Only source files here!
project(libACE LANGUAGES CXX)

#Create a library called "Hello" which includes the source file "hello.icxx".
# The extension is already found. Any number of sources could be listed here.
add_library ( ACE 
	      device/Device.cpp
	      device/MemoryResource.cpp
	      device/Types.cpp
	      device/numa/Device.cpp
	      device/numa/MemoryResource.cpp
	      device/numa/Scheduler.cpp
	      thread/Barrier.cpp
	      thread/Exception.cpp
	      thread/Mutex.cpp
	      thread/Pool.cpp
	      thread/Thread.cpp
	      utility/LockGuard.cpp
	      )

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(ACE 
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
   	       $<INSTALL_INTERFACE:include>
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ACE
	pthread
	numa)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS ACE EXPORT ACEConfig
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the build directory
export(TARGETS ACE FILE ACEConfig.cmake)
