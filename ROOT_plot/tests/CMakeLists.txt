# Get Catch2
include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(
	testRingBuffer
	${CMAKE_CURRENT_SOURCE_DIR}/RingBuffer.test.cpp
)
target_link_libraries(testRingBuffer PRIVATE Catch2::Catch2WithMain)
target_include_directories(testRingBuffer PRIVATE ${SRC_DIR})
add_test(NAME testRingBuffer COMMAND testRingBuffer)
catch_discover_tests(testRingBuffer)

add_executable(
	testLockableStream
	${CMAKE_CURRENT_SOURCE_DIR}/LockableStream.test.cpp
	${SRC_DIR}/DAQMonitor/LockableStream.cpp
	${SRC_DIR}/DAQMonitor/FileManagement/FileManager.cpp
)
target_link_libraries(testLockableStream PRIVATE Catch2::Catch2WithMain)
target_include_directories(testLockableStream PRIVATE ${SRC_DIR})
add_test(NAME testLockableStream COMMAND testLockableStream)
catch_discover_tests(testLockableStream)