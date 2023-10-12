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

#include "macros/DAQState.cpp"
#include "macros/ErrorLogger.cpp"
#include "macros/UIFramework/UIException.cpp"
#include "macros/UIWindows/EntryWindow/EntryView.cpp"
#include "macros/UIWindows/AlertBox/AlertOperations.cpp"

#include "src/ProgramControl/Terminator.cpp"
#include "src/ProgramControl/SigHandlers.cpp"
#include "src/ProgramControl/Threads.cpp"

using namespace std;
using namespace Muon;

// TODO: Look at this for using QT in root:
//       https://root.cern/doc/v606/QtFileDialog_8C.html

const string STATE_STORAGE = "settings.txt";

void StartDAQ() {

    // TODO: We want a console logger class to replace cout too.
    ErrorLogger::getInstance().setOutputStream(cerr);

    // Read in DAQState from file
    State::DAQState state = State::DAQState::getState();
    state.load(STATE_STORAGE);
    state.commit();

    // THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
    // It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly terminate threads.
    setTerminationHandlers(flagForTermination);

    ///////////////////////////////////////////////////////////////////////////
    //////////////////////////// SET UP UI ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(

        thread ([](){

            // Create the main window
            TGMainFrame *mainFrame = new TGMainFrame(gClient->GetRoot());

            // Create the menu displayed on the main window
            EntryView *menu = new EntryView(mainFrame, 500, 350);
            mainFrame->AddFrame(menu, new TGLayoutHints(kLHintsCenterX));

            // Set up the main window now that it has all its components
            mainFrame->SetWindowName("DAQ Manager");
            mainFrame->MapSubwindows();
            mainFrame->Resize(mainFrame->GetDefaultSize());
            mainFrame->MapWindow();

            // Wire up the main window's close button to the terminator
            mainFrame->Connect(
                "CloseWindow()", 
                "EntryOperations", 
                nullptr, 
                "exitAll()"
            );

            // Run the UI update loop
            while(!Terminator::getInstance().isTerminated()) {

                try {

                    gSystem->ProcessEvents();

                } catch (UIException &e) {

                    // TODO: More sophisticated error handling. We should
                    //       be logging these errors too.
                    // TODO: Multiple types of error titles
                    Error::popupAlert(
                        gClient->GetRoot(), "Error", e.what()
                    );

                }

                this_thread::sleep_for(
                    chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE))
                );

            }

            // TODO: Make sure all windows are closed without double deletion.
            //       Otherwise we will find that the GUI will stay open until
            //       every thread terminates.
            //         -- Really though we want the data capture thread to
            //            terminate immediately
            //         -- A window manager might be useful here. ExitAll
            //            can delete all the windows, and then we can just
            //            rewire the normal CloseWindow signal

        })

    );
    ProgramFlow::threadLock.unlock();

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////////////// CLEANUP ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Wait for the threads to be done before terminating
    ProgramFlow::joinAllThreads();

    cout << "Processed " << DAQData::getInstance().processedEvents.size() << " events." << endl;
    
    // Save DAQState to file
    //   -- TODO: Consider saving DAQState on startRun too
    state.update();
    state.save(STATE_STORAGE);

    cout << "Shutdown" << endl;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

}