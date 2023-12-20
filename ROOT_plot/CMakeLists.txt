cmake_minimum_required(VERSION 2.8.12)
project(DAQManager)

#
# Include the ROOT library
#

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Net Gui)

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

# add_executable(DAQManager StartDAQ.cpp)

add_executable(p2Decode DAQMonitor/PacketDecoding/Phase2Decode_Standalone.cpp)
target_link_libraries(p2Decode PUBLIC ${ROOT_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")