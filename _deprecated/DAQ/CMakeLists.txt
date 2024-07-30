# CMakeLists.xt for smdt package
cmake_minimum_required(VERSION 2.8.10)
project(MiniDAQ)

SET(CMAKE_CXX_COMPILER "/home/rpm/gcc-6.2.0/gcc-build/bin/g++")

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11 -g -Wall")
SET(GCC_COVERAGE_LINK_FLAGS    "-lpcap")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")




# #
# # Include the ROOT library
# #

# list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)

# message(${ROOT_INCLUDE_DIRS})
# include_directories(${ROOT_INCLUDE_DIRS})

# #---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
# include(${ROOT_USE_FILE})

# # #
# # # Define which files are headers and source
# # #

file(GLOB MiniDAQ_HEADERS ${PROJECT_SOURCE_DIR}/EthernetCap.h)
file(GLOB MiniDAQ_SOURCES  ${PROJECT_SOURCE_DIR}/EthernetCap.cpp)


set(daqObjs_HEADERS
EthernetCap.h 
)

set(daqObjs_SOURCES
EthernetCap.cpp
)


# #
# # Link this library to ROOT
# #

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ROOT_GENERATE_DICTIONARY(G__miniDAQ_CINT ${daqObjs_HEADERS} LINKDEF LinkDef.h)

# add_library(miniDAQ_CINT SHARED ${daqObjs_SOURCES} ${daqObjs_HEADERS} G__miniDAQ_CINT.cxx)

# add_library(miniDAQ SHARED ${MiniDAQ_SOURCES} G__miniDAQ_CINT.cxx)

add_executable(mac_daq EthernetCap.cpp)
# target_link_libraries(mac_daq miniDAQ ${ROOT_LIBRARIES})
