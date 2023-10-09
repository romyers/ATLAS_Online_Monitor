/**
 * @file StartDAQ.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <thread>
#include <mutex>

#include "monitorConfig.cpp"

#include "macros/UIFramework/GUIMainFrame.cpp"

#include "macros/UIWindows/EntryWindow/EntryMenu.cpp"

#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/SigHandlers.cpp"

using namespace std;
using namespace Muon;

// TODO: Look at this for using QT in root:
//       https://root.cern/doc/v606/QtFileDialog_8C.html

// TODO: This needs a better place to live
mutex UILock;


void StartDAQ() {

    // TODO: Set up default settings and settings persistence

    // THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
    // It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly terminate threads.
    setTerminationHandlers(flagForTermination);

    // Start the UI update thread
    thread UIThread([](){

        // TODO: Terminate this loop on exit conditions
        while(!Terminator::getInstance().isTerminated()) {

            UILock.lock();
            gSystem->ProcessEvents();
            UILock.unlock();

            this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

        }

        // TODO: Teardown UI cleanly without killing all other threads

    });

    // TODO: Open main menu

    UILock.lock();
    GUIMainFrame *mainFrame = new GUIMainFrame(gClient->GetRoot());

    EntryMenu *menu = new EntryMenu(mainFrame, 500, 500);
    mainFrame->AddFrame(menu, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

    mainFrame->SetWindowName("DAQ Manager");
    mainFrame->MapSubwindows();
    mainFrame->Resize(mainFrame->GetDefaultSize());
    mainFrame->MapWindow();

    UILock.unlock();

    // Wait for the threads to be done before terminating
    UIThread.join();

    mainFrame->Cleanup();
    delete mainFrame;
    mainFrame = nullptr;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

}