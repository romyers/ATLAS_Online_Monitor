#include "BuildGUI.h"

#include "Core/UILoop.h"

#include <iostream>

using namespace std;
using namespace Muon;

DAQManager *buildGUI() {

    // Create the main window
    DAQManager *menu = new DAQManager(gClient->GetRoot(), 1, 1, kVerticalFrame);

    // Set up the main window now that it has all its components
    menu->SetWindowName("DAQ Manager");
    menu->MapSubwindows();
    menu->Resize(menu->GetDefaultSize());
    menu->MapWindow();

    return menu;

}

void startUILoop(double refreshRate) {

    startLoop(refreshRate);

}

void stopUILoop() {

    stopLoop();

}