#include "SignalHandlers.h"

#include <string>

#include "DAQMonitor/DataRunOperations.h"
#include "DAQMonitor/DAQState.h"
#include "DAQMonitor/DataModel/DAQData.h"

#include "Logging/ErrorLogger.h"

#include "GUI/BuildGUI.h"
#include "GUI/Core/UISignals.h"
#include "GUI/Core/UIException.h"
#include "GUI/Components/UpdatePacket.h"

using namespace std;
using namespace Muon;

// FIXME: This global is NOT what I want to be doing. Come up with a better
//        way to push data updates.
DAQManager *GUIptr = nullptr;

void connectDAQto(DAQManager *GUI) {

	GUIptr = GUI;

	GUI->Connect(
		"pressedStart()",
		"Muon::SigHandlers",
		nullptr,
		"handlePressedStartRun()"
	);

	GUI->Connect(
		"pressedStop()",
		"Muon::SigHandlers",
		nullptr,
		"handlePressedStopRun()"
	);

    GUI->Connect(
        "pressedExit()",
        "Muon::SigHandlers",
        nullptr,
        "handleExit()"
    );

    GUI->Connect(
        "CloseWindow()",
        "Muon::SigHandlers",
        nullptr,
        "handleExit()"
    );

    GUI->Connect(
    	"selectedDeviceSource()",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleSelectedDeviceSource()"
    );

    GUI->Connect(
    	"selectedFileSource()",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleSelectedFileSource()"
    );

    GUI->Connect(
    	"selectedDevice(const char*)",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleSelectedDevice(const char*)"
    );

    GUI->Connect(
    	"selectedFile(const char*)",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleSelectedFile(const char*)"
    );

    // TODO: This is an odd one out. We should only be connecting
    //       GUI signals in this function.
    UISignalBus::getInstance().Connect(
    	"onUpdate()", 
    	"Muon::SigHandlers", 
    	nullptr, 
    	"handleDataUpdate()"
    );

    UISignalBus::getInstance().Connect(
    	"onRunStart()",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleRunStartEvent()"
    );

    UISignalBus::getInstance().Connect(
    	"onRunStop()",
    	"Muon::SigHandlers",
    	nullptr,
    	"handleRunStopEvent()"
    );

}

void SigHandlers::handlePressedStartRun() {

	State::DAQState::getState().save();

	GUIptr->disableStartButton();

	try{

		DataRun::startRun();

	} catch(UIException &e) {

		GUIptr->enableStartButton();

		throw e;

	}

}

void SigHandlers::handleRunStartEvent() {

	if(!GUIptr) return;

	GUIptr->enableStopButton();
	GUIptr->disableDataSourcePanel();

}

void SigHandlers::handlePressedStopRun() {

	GUIptr->disableStopButton();

	DataRun::stopRun();

}

void SigHandlers::handleRunStopEvent() {

	if(!GUIptr) return;

	GUIptr->enableStartButton();
	GUIptr->enableDataSourcePanel();

}

void SigHandlers::handleExit() {

	if(DataRun::isRunning()) DataRun::stopRun();

	GUIptr->disable();

	stopUILoop();

}

void SigHandlers::handleSelectedFileSource() {
    
    State::DAQState state = State::DAQState::getState();

    state.persistentState.dataSource = State::DAT_FILE_SOURCE;

    state.commit();

}

void SigHandlers::handleSelectedDeviceSource() {

    State::DAQState state = State::DAQState::getState();

    state.persistentState.dataSource = State::NETWORK_DEVICE_SOURCE;

    state.commit();

}

void SigHandlers::handleSelectedDevice(const char* selection) {

    State::DAQState state = State::DAQState::getState();

    state.persistentState.inputDevicename = string(selection);

    state.commit();

}

void SigHandlers::handleSelectedFile(const char* selection) {

    State::DAQState state = State::DAQState::getState();

    state.persistentState.inputFilename = selection;

    state.commit();

}

void SigHandlers::handleDataUpdate() {

	if(!GUIptr) return;

	UpdatePacket packet;

	DAQData     &data   = DAQData    ::getInstance();
	ErrorLogger &logger = ErrorLogger::getInstance();

	data.lock();
	packet.packetCount     = data.packetCount                                  ;
	packet.totalEventCount = data.totalEventCount                              ;
	packet.emptyEventCount = data.totalEventCount - data.processedEvents.size();
	packet.lostPackets     = data.lostPackets                                  ;
	packet.droppedSignals  = data.droppedSignals                               ;
	packet.droppedEvents   = data.droppedEvents                                ;
	data.unlock();

	packet.errorCount      = logger.countErrors(EMPTY_TYPE, ERROR)             ;
	packet.warningCount    = logger.countErrors(EMPTY_TYPE, WARNING)           ;

	State::DAQState state = State::DAQState::getState();
	if(state.tempState.runLabel != "") {

		if(DataRun::isRunning()) {

			packet.statusTag = string("Started Run: ") + state.tempState.runLabel;

		} else {

			packet.statusTag = string("Stopped Run: ") + state.tempState.runLabel;

		}

	}

	GUIptr->update(packet);

}