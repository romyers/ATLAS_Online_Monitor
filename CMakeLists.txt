cmake_minimum_required(VERSION 3.22.1)
project(
    DAQMonitor
    VERSION 1.0
    DESCRIPTION "Online Monitor for the miniDAQ System"
    LANGUAGES C CXX
)

# We only want to do these things if we are the top-level project
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

    # Build tests
    if(BUILD_TESTING)
    
        include(CTest)
        add_subdirectory(tests)

    endif()

endif()

# Find ROOT
# NOTE: More details on CMake with ROOT can be found here:
#       https://cliutils.gitlab.io/modern-cmake/chapters/packages/ROOT.html

message(Using\ g++\ version\ ${CMAKE_CXX_COMPILER_VERSION})

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Net Gui GuiHtml)

message(ROOT\ include\ directory:\ ${ROOT_INCLUDE_DIRS})

# Build libraries

set(
    DAQGUI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonitorView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DAQApp.h
)

set(
    DAQGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonitorView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DAQApp.cpp
)

# This makes signals and slots work
root_generate_dictionary(
    G__UISignals
    ${DAQGUI_HEADERS}
    LINKDEF
    ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
)

add_library(
    DAQGUI
    SHARED
    ${DAQGUI_SOURCES}
    G__UISignals.cxx
)
target_link_libraries(
    DAQGUI
    PUBLIC
    ROOT::Core
    ROOT::RIO
    ROOT::Hist
    ROOT::Tree
    ROOT::Net
    ROOT::Gui
    ROOT::GuiHtml
)

# Build and link the executable
add_executable(
    DAQMonitor
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(
    DAQMonitor
    PRIVATE
    DAQGUI
)