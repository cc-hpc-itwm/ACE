#Create a library called "Hello" which includes the source file "hello.icxx".
# The extension is already found. Any number of sources could be listed here.
add_library ( ACE 
#	      scheduler_Schedule.cpp
	      scheduler_Table.cpp
#	      scheduler_Thread.cpp
#	      scheduler_Threading.cpp
	      scheduler_Timers.cpp
#	      ScheduleExecuter.cpp
	      Thread.cpp
	      ThreadPool.cpp
	      utility/LockGuard.cpp
	      )

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (ACE 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include 
	       ${CMAKE_CURRENT_SOURCE_DIR} )
