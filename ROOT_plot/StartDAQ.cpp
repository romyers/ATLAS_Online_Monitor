/**
 * @file StartDAQ.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: StartDAQ blocks SIGINT. Termination can be forced by
 *       typing CTRL+C twice.
 */

// TODO: Consider an operator pattern where the operations file maintains a
//       global pointer to the associated view and implements an initializer

#pragma once

#include <thread>
#include <mutex>

#include "monitorConfig.cpp"

#include "macros/UIFramework/GUIMainFrame.cpp"

#include "macros/UIWindows/EntryWindow/EntryView.cpp"
#include "macros/UIWindows/EntryWindow/EntryOperations.cpp"

#include "macros/UIWindows/LogWindow/LogOperations.cpp"

#include "macros/UIWindows/AlertBox/AlertOperations.cpp"

#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/SigHandlers.cpp"
#include "src/ProgramControl/Threads.cpp"

using namespace std;
using namespace Muon;

// TODO: Look at this for using QT in root:
//       https://root.cern/doc/v606/QtFileDialog_8C.html

// TODO: This needs a better place to live

const string STATE_STORAGE = "settings.txt";

void StartDAQ() {

    // TODO: Set up default settings and settings persistence

    // THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
    // It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly terminate threads.
    setTerminationHandlers(flagForTermination);

    EntryOperations::readState(STATE_STORAGE);

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////// START UI UPDATE THREAD /////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    mutex UILock;
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(

        thread ([](){

            GUIMainFrame *mainFrame = new GUIMainFrame(gClient->GetRoot());

            EntryView *menu = new EntryView(mainFrame, 500, 350);
            mainFrame->AddFrame(menu, new TGLayoutHints(kLHintsCenterX));

            mainFrame->SetWindowName("DAQ Manager");
            mainFrame->MapSubwindows();
            mainFrame->Resize(mainFrame->GetDefaultSize());
            mainFrame->MapWindow();

            // TODO: Terminate this loop on exit conditions
            while(!Terminator::getInstance().isTerminated()) {

                try {

                    gSystem->ProcessEvents();

                } catch (UIException &e) {

                    Error::popupAlert(gClient->GetRoot(), "UI Error", e.what());

                }

                this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

            }

            mainFrame->Cleanup();

            delete mainFrame;
            mainFrame = nullptr;

        })

    );

    ProgramFlow::threadLock.unlock();

    ///////////////////////////////////////////////////////////////////////////
    /////////////////////// SET UP ENTRY UI WINDOW ////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////////////// CLEANUP ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Wait for the threads to be done before terminating
    ProgramFlow::joinAllThreads();
    
    EntryOperations::saveState(STATE_STORAGE);

    cout << "Shutdown" << endl;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

}