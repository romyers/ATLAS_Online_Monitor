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

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_executable(
	testDecoder 
	Decoder.test.cpp 
)
target_link_libraries(testDecoder PRIVATE DAQDecode Catch2::Catch2WithMain)
target_include_directories(testDecoder PRIVATE ${INCLUDE_DIR})
add_test(NAME testDecoder COMMAND testDecoder)
catch_discover_tests(testDecoder)