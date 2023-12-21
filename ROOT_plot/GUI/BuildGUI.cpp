#include "BuildGUI.h"

#include "Core/UILoop.h"

#include "Components/DAQManager.h"

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
        "DAQManager", 
        menu, 
        "handlePressExit()"
    );

    return mainFrame;

}

void startUILoop(double refreshRate) {

    startLoop(refreshRate);

}