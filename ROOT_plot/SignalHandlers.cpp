#include "SignalHandlers.h"

#include "DAQMonitor/DataRunOperations.h"
#include "DAQMonitor/DAQState.h"

#include "GUI/BuildGUI.h"

using namespace std;
using namespace Muon;

void SigHandlers::handleStartRun() {

	DataRun::startRun();

}

void SigHandlers::handleStopRun() {

	DataRun::stopRun();

}

void SigHandlers::handleExit() {

	// TODO: Cleaner way would be to check if the run is in progress
	//       directly
	try{
		DataRun::stopRun();
	} catch(const exception &e) {}

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

void connectDAQto(TGMainFrame *GUI) {

	GUI->Connect(
		"pressedStart()",
		"Muon::SigHandlers",
		nullptr,
		"handleStartRun()"
	);

	GUI->Connect(
		"pressedStop()",
		"Muon::SigHandlers",
		nullptr,
		"handleStopRun()"
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

}