/**
 * @file StartDAQ.cpp
 *
 * @brief Entry point for the Phase 2 MiniDAQ online monitor.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: StartDAQ blocks SIGINT. Termination can be forced by
 *       entering CTRL+C twice.
 */

// TODO: Consider an operator pattern where the operations file maintains a
//       global pointer to the associated view and implements an initializer

#include <thread>
#include <mutex>

#include "TApplication.h"
#include "TUnixSystem.h" // Needed for calls to gSystem

#include "GUI/BuildGUI.h"

#include "DAQMonitor/DAQState.h"
#include "DAQMonitor/ProgramControl/SigHandlers.h"
#include "DAQMonitor/ProgramControl/Threads.h"

using namespace std;
using namespace Muon;


// COPIED TODOS FROM START_MONITOR:
// TODO: Rethink the semantics of Monitor. We've made our labels as if it's a
//       display element when really it does data decoding. It should be the
//       UI element that does the 'refreshing', for example.

// TODO: Examine this for ROOT tips:
//       https://mightynotes.wordpress.com/2020/02/15/cern-root-tips-and-tricks/

// TODO: Call getpid() for the run? Do we need to give it a pid?

// TODO: Config GUI for options like whether to show lost packets

// TODO: Split out all cout calls to a console logger object. Easy to make it
//       threadsafe or switch the stream we're logging to

// TODO: Make the error logger threadsafe

// TODO: Set Geometry::runN as in DecodeOffline.cpp

// TODO: Look at this for using QT in root:
//       https://root.cern/doc/v606/QtFileDialog_8C.html

/**
 * The settings file providing persistent storage for DAQState.
 */
const string STATE_STORAGE = "settings.txt";

/**
 * The approximate rate at which the GUI is refreshed.
 */
const double GUI_REFRESH_RATE  = 60.; //Hz

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main() {

    // NOTE: This appears to populate the global gApplication and gSystem 
    //       variables.
    TApplication app("DAQManager", nullptr, nullptr);

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

    // Create the GUI for the online monitor
    buildGUI();

    // Start the UI event loop thread
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(

        thread([]() {

            startUILoop(GUI_REFRESH_RATE);

        })

    );
    ProgramFlow::threadLock.unlock();

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////////////// CLEANUP ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Wait for all threads to be done before terminating
    ProgramFlow::joinAllThreads();
    
    // Save DAQState to file
    //   -- TODO: Consider saving DAQState on startRun or when user applies
    //            changes to DAQState.
    state.update();
    state.save(STATE_STORAGE);

    cout << "Shutdown" << endl;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

    return 0;

}