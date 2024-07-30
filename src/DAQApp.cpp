#include "DAQApp.h"

#include "TROOT.h"

// Populates globals like gSystem
// TODO: Check OS and use appropriate header.
//       We need a different header for Windows (and possibly Mac).
#include "TUnixSystem.h"

// TODO: This is the only direct UI element in the DAQApp. Is this the
//       right place for it?
#include "TGMsgBox.h"

#include <thread>
#include <chrono>
#include <stdexcept>

using namespace DAQ;

ClassImp(App);

App::App(const std::string &title) 
    : app(title.data(), nullptr, nullptr) {

    // Configure ROOT for multithreading
    ROOT::EnableThreadSafety();
    ROOT::EnableImplicitMT();

}

App::~App() {

    gApplication->Terminate(0);

}

void App::run(double refreshRate) {

    while (runLoop) {

        try {

            gSystem->ProcessEvents();

        } catch(const std::exception &e) {

            // TODO: Second argument should be the main frame
            //         -- A reason to make App a TGMainFrame?
            //         -- Another reason -- we get to hide the connection
            //            between closeWindow() and stop()
            TGMsgBox *errorBox = new TGMsgBox(
                gClient->GetRoot(),
                nullptr,
                "Not so fast!",
                e.what(),
                kMBIconExclamation,
                kMBOk
            );

        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                (int)(1000 / refreshRate)
            )
        );

    }
    
    // Reset runLoop so that subsequent calls to run() will work
    runLoop = true;

}

void App::stop() {

    runLoop = false;

}