/**
 * @file StartGUI.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// https://root.cern.ch/root/htmldoc/guides/users-guide/WritingGUI.html#event-processing-signals-and-slots

#pragma once

#include <thread>
#include <map>

#include "monitorConfig.cpp"

#include "macros/UIFramework/UIManager.cpp"
#include "macros/UIFramework/Panel.cpp"
#include "macros/UIFramework/UIElement.cpp"
#include "macros/UIFramework/UIException.cpp"
#include "macros/UIFramework/RadioPanel.cpp"
#include "macros/UIFramework/GUIMainFrame.cpp"

using namespace std;

// Signal handler that quits root for us
void handler(int sig) {

    gApplication->Terminate(0);

}

// TODO: Make sure any created panels don't go out of scope until we're done
//       with them.
void StartGUI() {

    // Intercept signals so we stop hanging if we ctrl+c
    struct sigaction signalHandler;
    sigemptyset(&signalHandler.sa_mask);
    signalHandler.sa_flags = 0;
    signalHandler.sa_handler = handler;

    sigaction(SIGTERM, &signalHandler, NULL);
    sigaction(SIGINT , &signalHandler, NULL);
    sigaction(SIGQUIT, &signalHandler, NULL);

    GUIMainFrame *frame = new GUIMainFrame(gClient->GetRoot());

    RadioPanel *radioSelector = new RadioPanel(frame, "Select Device", kVerticalFrame);
    radioSelector->addOption("Device 1");
    radioSelector->addOption("Device 2");
    radioSelector->addOption("Device 3");
    radioSelector->setDefaultOption("Device 2");
    frame->AddFrame(radioSelector, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    frame->SetWindowName("Selector");
    frame->MapSubwindows();
    frame->Resize(frame->GetDefaultSize());
    frame->MapWindow();

    while(true) {

        gSystem->ProcessEvents();

    }



    /*

    UIManager manager;

    HelloSayer sayer;

    Panel panel ("c1", "Test" , 100, 100, 400, 400);
    Panel panel2("c2", "Test2", 600, 600, 400, 400);

    manager.attach(panel);
    manager.attach(panel2);


    UIElement button (new TButton("Button" , "7+1", 0.1, 0.1, 0.5, 0.2));
    UIElement button2(new TButton("Button2", "5+2", 0.1, 0.1, 0.5, 0.2));

    panel2.attach(button);
    panel.attach(button2);

    while(true) {

        manager.refresh();

        this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

    }

    */

}