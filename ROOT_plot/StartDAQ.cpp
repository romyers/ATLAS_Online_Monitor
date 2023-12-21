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
#include <chrono>
#include <cmath>

#include "TApplication.h"
#include "TUnixSystem.h"

#include "TGMenu.h"

#include "macros/DAQState.h"
#include "macros/UIFramework/UIException.h"
#include "macros/UIFramework/UILock.h"
#include "macros/UIWindows/AlertBox/AlertOperations.h"

#include "MainMenu/Views/EntryView.h"

#include "src/ProgramControl/SigHandlers.h"
#include "src/ProgramControl/Threads.h"
#include "src/ProgramControl/Terminator.h"

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

const string STATE_STORAGE = "settings.txt";

/**
 * The approximate rate at which the GUI is refreshed.
 */
const double GUI_REFRESH_RATE  = 60.; //Hz

/**
 * Update UI elements every [UI_UPDATE_FRAMES] frames, i.e.
 * GUI_REFRESH_RATE / UI_UPDATE_FRAMES times per second.
 */
const int    UI_UPDATE_FRAMES  = 15  ; // Frames

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main() {
    // NOTE: This appears to populate the global gApplication variable.
    TApplication app("DAQManager", nullptr, nullptr);

    // And this appears to populate gSystem.
    TUnixSystem system;

    // TODO: It might be nice to have a more general 'logger' that stores
    //       everything, not just errors.
    //         -- Can just add a new error 'type'

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

    // TODO: Explore using app.Run() instead of manually calling 
    //       ProcessEvents()

    // TODO: Move this to EntryOperations
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(

        thread([](){

            // Create the main window
            TGMainFrame *mainFrame = new TGMainFrame(gClient->GetRoot());

            // Create the menu displayed on the main window
            EntryView *menu = new EntryView(mainFrame, 1, 1, kVerticalFrame);
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
            // TODO: Check Resource Red to remember how to properly track
            //       dropped frames
            // TODO: Now that the logic is a little more complicated, pull it
            //       out
            // TODO: Include a setting regarding whether to warn for slow
            //       frames
            // TODO: A lot of this framerate stuff really is overkill. 
            double slowFrames = 0;
            while(!Terminator::getInstance().isTerminated()) {

                // Record the loop start time
                auto UIUpdateStartTime = chrono::high_resolution_clock::now();


                // TODO: This locks the UI lock for the entire event 
                //       processing loop. Would be nice if we didn't block
                //       other UI updates for the WHOLE loop.
                Muon::UI::UILock.lock();
                try {

                    gSystem->ProcessEvents();

                } catch (UIException &e) {

                    Error::popupAlert(
                        gClient->GetRoot(), "Error", e.what()
                    );

                }
                Muon::UI::UILock.unlock();

                // Record the loop end time
                auto UIUpdateEndTime = chrono::high_resolution_clock::now();

                // Compute how long it took the loop to run
                chrono::duration<double> loopTime = UIUpdateEndTime - UIUpdateStartTime;
                chrono::milliseconds updateTime = chrono::duration_cast<chrono::milliseconds>(loopTime);

                State::DAQState state = State::DAQState::getState();

                bool warnSlowFrames = state.persistentState.warnSlowFrames;

                // Compute how long the thread should sleep to maintain a 
                // consistent frame rate
                int sleepTime = (int)(1000. / GUI_REFRESH_RATE) - (int)(updateTime.count());

                // Deal with the case where the update loop took too long
                if(sleepTime < 0) {

                    // TODO: I think better if we increment slowFrames by a
                    //       measure of how slow the frame was, so e.g.
                    //       we don't suppress the warnings if we only
                    //       update the UI every ten frames.

                    if(warnSlowFrames) ++slowFrames;
                    sleepTime = 0;

                }

                if(warnSlowFrames) {

                    // If we are slow more than 5 times before slowFrames decays
                    // we should alert the user.
                    if(slowFrames > state.persistentState.slowFrameTolerance) {

                        cerr << "UI WARNING: GUI refresh rate too fast for data updates. "
                             << "Consider reducing GUI update framerate." << endl;

                    }

                    // Linear decay rate for slowFrames
                    slowFrames -= state.persistentState.slowFrameDecayRate;
                    slowFrames = max(slowFrames, 0.);

                }

                this_thread::sleep_for(
                    chrono::milliseconds(sleepTime)
                );

            }

        })

    );
    ProgramFlow::threadLock.unlock();

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////////////// CLEANUP ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Wait for the threads to be done before terminating
    ProgramFlow::joinAllThreads();
    
    // Save DAQState to file
    //   -- TODO: Consider saving DAQState on startRun too
    state.update();
    state.save(STATE_STORAGE);

    cout << "Shutdown" << endl;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

    return 0;

}