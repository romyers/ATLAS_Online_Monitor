#include "BuildGUI.h"

#include "Core/UILoop.h"

#include "Components/DAQManager.h"

#include "DAQMonitor/DataRunOperations.h"
#include "DAQMonitor/ProgramControl/Terminator.h"

#include <iostream>

using namespace std;
using namespace Muon;

void SigHandlers::handlePressedStart() {

    DataRun::startRun();

}

void SigHandlers::handlePressedStop() {

    DataRun::stopRun();

}

void SigHandlers::handleExit() {

    Terminator::getInstance().terminate();

}

TGMainFrame *buildGUI() {

    // Create the main window
    TGMainFrame *mainFrame = new TGMainFrame(gClient->GetRoot());

    // Create the menu displayed on the main window
    DAQManager *menu = new DAQManager(mainFrame, 1, 1, kVerticalFrame);
    mainFrame->AddFrame(menu, new TGLayoutHints(kLHintsCenterX));

    // Set up the main window now that it has all its components
    mainFrame->SetWindowName("DAQ Manager");
    mainFrame->MapSubwindows();
    mainFrame->Resize(mainFrame->GetDefaultSize());
    mainFrame->MapWindow();

    // Wire up the main window's close button to the terminator
    mainFrame->Connect(
        "CloseWindow()", 
        "SigHandlers", 
        nullptr, 
        "handleExit()"
    );

    menu->Connect(
        "pressedStart()",
        "SigHandlers",
        nullptr,
        "handlePressedStart()"
    );

    menu->Connect(
        "pressedStop()",
        "SigHandlers",
        nullptr,
        "handlePressedStop()"
    );

    menu->Connect(
        "pressedExit()",
        "SigHandlers",
        nullptr,
        "handleExit()"
    );

    return mainFrame;

}

void startUILoop(double refreshRate) {

    startLoop(refreshRate);

}