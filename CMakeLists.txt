cmake_minimum_required(VERSION 3.22.1)
project(
	DAQMonitor
	VERSION 1.0
	DESCRIPTION "Online Monitor for the miniDAQ System"
	LANGUAGES C CXX
)

# Add libraries
include(ExternalProject)
# add_subdirectory(external/SMDT_RECO)
# add_subdirectory(lib/DAQDecode)
# ExternalProject_Add(
#	MuonReco
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SMDT_RECO
#	INSTALL_COMMAND cmake -E echo "Skipping install step."
# )
# ExternalProject_Add(
#	DAQDecode
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/DAQDecode
#	INSTALL_COMMAND cmake -E echo "Skipping install step."
#)
# add_subdirectory(lib/Ethernet_Capture)

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


# ROOT won't can't generate dictionaries correctly without this
include(GNUInstallDirs)

# Root_Generate_Dictionary will fail if there are no globally defined
# include directories, so we make a nonexistent one. Supposedly this
# was fixed after ROOT 6.16, but I've found that we still need this
# for ROOT 6.30
include_directories(ROOT_NONEXISTENT_DIRECTORY_HACK)

# Build libraries

# DEVELOPER NOTE: If you define a new class using signals and slots, you must
# follow the steps included in the top comment on LinkDef.h or the signals and
# slots will not connect.
set(
	DAQGUI_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/MonitorView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/DAQApp.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/FileSelector.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/RunControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/RunView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SettingsPanel.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/TabPanel.h
)

set(
	DAQGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/MonitorView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/DAQApp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FileSelector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/RunControl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/RunView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SettingsPanel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TabPanel.cpp
)

# This makes signals and slots work in the DAQGUI library
# See: https://github.com/eguiraud/root_dictionaries_tutorial?tab=readme-ov-file#the-cmake-way
root_generate_dictionary(
	G__UISignals
	${DAQGUI_HEADERS}
	LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
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

# TODO: It would be better to take the icon from the root icon folder, but
#       we have to recover the path to that folder from ROOT_INCLUDE_DIRS
# Let's make sure that the executable can find the program's icons
# no matter where it is compiled to
# SOURCE: https://qrikko.blogspot.com/2016/05/cmake-and-how-to-copy-resources-during.html
set(source "${CMAKE_CURRENT_SOURCE_DIR}/icons")
set(destination "${CMAKE_CURRENT_BINARY_DIR}/icons")
add_custom_command(
	TARGET DAQMonitor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
	DEPENDS ${destination}
	COMMENT "Creating symlink to icon directory"
)
