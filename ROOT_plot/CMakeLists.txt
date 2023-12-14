# CMakeLists.xt for smdt package
cmake_minimum_required(VERSION 2.8.10)
project(DAQManager)


#
# Include the ROOT library
#

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net Gui)


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(DAQManager StartDAQ.cpp)

add_executable(helloWorld macros/test.cpp)
target_link_libraries(helloWorld PUBLIC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")