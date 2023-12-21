cmake_minimum_required(VERSION 2.8.12)
project(DAQManager)

# Point CMAKE at the project directory when it looks for e.g. FindPCAP.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/")

#
# Include the ROOT library
#

message(${CMAKE_CXX_COMPILER_VERSION})

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Net Gui GuiHtml)
find_package(PCAP REQUIRED)

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

set(recoObjs_HEADERS 
	src/EventID.h
	src/Signal.h
	src/Cluster.h
	src/Hit.h
	src/Event.h
	src/Track.h
	src/Geometry.h
)

set(recoObjs_SOURCES
	src/EventID.cpp
	src/Signal.cpp
	src/Cluster.cpp
	src/Hit.cpp
	src/Event.cpp
	src/Track.cpp
	src/Geometry.cpp
)

set(p2Core_HEADERS
	macros/ErrorLogger.h
	DAQMonitor/LockableStream.h
)

set(p2Core_SOURCES
	macros/ErrorLogger.cpp
	DAQMonitor/LockableStream.cpp
)

set(p2ecap_HEADERS
	DAQMonitor/EthernetCapture/src/DeviceManager.h
	DAQMonitor/EthernetCapture/src/DeviceSelector.h
	DAQMonitor/EthernetCapture/src/NetworkDeviceException.h
	DAQMonitor/EthernetCapture/src/PCapDevice.h
	DAQMonitor/EthernetCapture/src/PCapSessionHandler.h
)

set(p2ecap_SOURCES
	DAQMonitor/EthernetCapture/src/DeviceManager.cpp
	DAQMonitor/EthernetCapture/src/DeviceSelector.cpp
	DAQMonitor/EthernetCapture/src/NetworkDeviceException.cpp
	DAQMonitor/EthernetCapture/src/PCapDevice.cpp
	DAQMonitor/EthernetCapture/src/PCapSessionHandler.cpp
)

set(p2Decode_HEADERS 
	DAQMonitor/PacketDecoding/src/Decoder.h
	DAQMonitor/PacketDecoding/src/EventDecoding.h
	DAQMonitor/PacketDecoding/src/SignalDecoding.h
)

set(p2Decode_SOURCES
	DAQMonitor/PacketDecoding/src/Decoder.cpp
	DAQMonitor/PacketDecoding/src/EventDecoding.cpp
	DAQMonitor/PacketDecoding/src/SignalDecoding.cpp
)

set(monitor_HEADERS
	src/DataModel/DAQData.h
	src/DataModel/Plots.h
	src/ProgramControl/SigHandlers.h
	src/ProgramControl/Threads.h
	src/ProgramControl/Terminator.h
	macros/DAQState.h
	macros/UIFramework/UIException.h
	macros/UIFramework/UILock.h
	macros/UIFramework/UISignals.h
	macros/UIWindows/AlertBox/AlertOperations.h
	MainMenu/Views/EntryView.h
	MainMenu/Views/Components/DataSourcePanel.h
	MainMenu/Views/Components/DeviceSelectorPanel.h
	MainMenu/Views/Components/FileSelectorPanel.h
	DAQMonitor/DataRunOperations.h
	DAQMonitor/Views/RunView.h
	DAQMonitor/Views/Components/RunStats.h
	DAQMonitor/Views/Components/RunMenuBar.h
	DAQMonitor/Views/Components/PairTable.h
	DAQMonitor/Views/Components/TabSelector.h
	DAQMonitor/Plotting/Views/PlotWindow.h
	DAQMonitor/Plotting/Views/GraphPlotter.h
	DAQMonitor/Plotting/Views/HistogramPlotter.h
	DAQMonitor/PlotSaving/PlotSavingOperations.h
	DAQMonitor/PlotSaving/Views/ProgressBar.h
	DAQMonitor/ErrorHandling/Views/ErrorView.h
	DAQMonitor/PacketDecoding/PacketDecodingOperations.h
	DAQMonitor/EthernetCapture/DataCaptureOperations.h
)

set(monitor_SOURCES
	src/DataModel/DAQData.cpp
	src/DataModel/Plots.cpp
	src/ProgramControl/SigHandlers.cpp
	src/ProgramControl/Threads.cpp
	src/ProgramControl/Terminator.cpp
	macros/DAQState.cpp
	macros/UIFramework/UIException.cpp
	macros/UIFramework/UILock.cpp
	macros/UIFramework/UISignals.cpp
	macros/UIWindows/AlertBox/AlertOperations.cpp
	MainMenu/Views/EntryView.cpp
	MainMenu/Views/Components/DataSourcePanel.cpp
	MainMenu/Views/Components/DeviceSelectorPanel.cpp
	MainMenu/Views/Components/FileSelectorPanel.cpp
	DAQMonitor/DataRunOperations.cpp
	DAQMonitor/Views/RunView.cpp
	DAQMonitor/Views/Components/RunStats.cpp
	DAQMonitor/Views/Components/RunMenuBar.cpp
	DAQMonitor/Views/Components/PairTable.cpp
	DAQMonitor/Views/Components/TabSelector.cpp
	DAQMonitor/Plotting/Views/PlotWindow.cpp
	DAQMonitor/Plotting/Views/GraphPlotter.cpp
	DAQMonitor/Plotting/Views/HistogramPlotter.cpp
	DAQMonitor/PlotSaving/PlotSavingOperations.cpp
	DAQMonitor/PlotSaving/Views/ProgressBar.cpp
	DAQMonitor/ErrorHandling/Views/ErrorView.cpp
	DAQMonitor/PacketDecoding/PacketDecodingOperations.cpp
	DAQMonitor/EthernetCapture/DataCaptureOperations.cpp
)

ROOT_GENERATE_DICTIONARY(G__UISignals ${monitor_HEADERS} LINKDEF LinkDef.h)


add_library(MuonReco ${recoObjs_SOURCES} ${recoObjs_HEADERS})
target_link_libraries(MuonReco ${ROOT_LIBRARIES})

add_library(libP2Core ${p2Core_SOURCES} ${p2Core_HEADERS})

add_library(libP2ecap ${p2ecap_SOURCES} ${p2ecap_HEADERS})
target_link_libraries(libP2ecap libP2Core ${PCAP_LIBRARY})

add_library(libP2Decode ${p2Decode_SOURCES} ${p2Decode_HEADERS})
target_link_libraries(libP2Decode libP2Core MuonReco)

add_library(libMonitor ${monitor_SOURCES} G__UISignals.cxx)
target_link_libraries(libMonitor libP2Decode libP2ecap)



add_executable(p2ecap DAQMonitor/EthernetCapture/Phase2EthernetCapture_Standalone.cpp)
target_link_libraries(p2ecap PUBLIC libP2ecap)

add_executable(p2decode DAQMonitor/PacketDecoding/Phase2Decode_Standalone.cpp)
target_link_libraries(p2decode PUBLIC libP2Decode)

add_executable(DAQManager StartDAQ.cpp)
target_link_libraries(DAQManager PUBLIC libMonitor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")