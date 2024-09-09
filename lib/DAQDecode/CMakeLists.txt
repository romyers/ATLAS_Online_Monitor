cmake_minimum_required(VERSION 3.22.1)
project(DAQDecode 
	VERSION 1.0 
	DESCRIPTION "Decoding library for the miniDAQ system"
	LANGUAGES C CXX
)

OPTION(BUILD_TESTING "Build the Test Suite" OFF)

include(FetchContent)

# We only want to do this if we are the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 

	set(CMAKE_CXX_EXTENSIONS OFF)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED True)

	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	find_package(Doxygen)
	if(Doxygen_FOUND)
		add_subdirectory(docs)
	else()
		message(STATUS "Doxygen not found, not building documentation")
	endif()

endif()

# Build the library
add_library(
	DAQDecode 
	SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/Decoder.cpp
)
add_dependencies(
	DAQDecode 
	MuonReco
)
target_link_libraries(
	DAQDecode
	PUBLIC
	MuonReco
)
target_include_directories(
	DAQDecode 
	PUBLIC 
	include
)

# Build tests
if(BUILD_TESTING)

	include(CTest)
	add_subdirectory(tests)

endif()