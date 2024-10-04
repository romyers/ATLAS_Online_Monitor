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

#include <thread>
#include <mutex>
#include <vector>
#include <string>

#include <iostream>

#include <getopt.h>

#include "TApplication.h"
#include "TUnixSystem.h" // Needed for calls to gSystem
#include "TGFrame.h"
#include "TStyle.h"

#include "SignalHandlers.h"

#include "GUI/BuildGUI.h"
#include "GUI/Components/DAQManager.h"

#include "DAQMonitor/DAQState.h"
#include "DAQMonitor/DataCaptureOperations.h"
#include "DAQMonitor/ProgramControl/SigHandlers.h"
#include "DAQMonitor/ProgramControl/Threads.h"

using namespace std;

// COPIED TODOS FROM START_MONITOR:

// TODO: Examine this for ROOT tips:
//       https://mightynotes.wordpress.com/2020/02/15/cern-root-tips

// TODO: Split out all cout calls to a console logger object. Easy to make it
//       threadsafe or switch the stream we're logging to

// TODO: Make the error logger threadsafe

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
const double DATA_REFRESH_RATE = 1.; // Hz

void forceExit(int signal) {

    gApplication->Terminate(0);

    exit(0); // In case gApplication->Terminate(0) doesn't do the job.

}

void termHandler(int signal) {

    SigHandlers::handleExit();

    setTerminationHandlers(forceExit);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void printUsage() {

	cout << "Usage: DAQManager [options]" << endl;
	cout << "Options:" << endl;
	cout << "  -s, --buffer-size <size>  Set the PCap buffer size in MB. Default is 100 MB." << endl;
	cout << "  -f, --font-size <size>    Set the font size for plot tabs, in pixels. Default is 11 pixels." << endl;
	cout << "  -h, --help                Display this help message" << endl;

}

int main(int argc, char **argv) {

	int pcapBufferSize = 100; // MB
	double fontSize = 11;     // Pixels

	const char *shortopts = "s:f:h";
	const struct option longopts[] = {
		{ "buffer-size", required_argument, nullptr, 's' },
		{ "font-size",   required_argument, nullptr, 'f' },
		{ "help",        no_argument,       nullptr, 'h' },
		{ nullptr, 0, nullptr, 0 }
	};

	while(optind < argc) {

		int opt = getopt_long(argc, argv, shortopts, longopts, nullptr);

		if(opt == -1) {

			break;

		}

		switch(opt) {

			case 's':

				try {

					pcapBufferSize = std::stoi(optarg);

					// Make sure the argument is an integer with no fractional part
					if(pcapBufferSize != std::stod(optarg)) {

						throw std::invalid_argument("Buffer size must be an integer.");

					}

					if(pcapBufferSize <= 0) {

						throw std::invalid_argument("Buffer size must be a positive integer.");

					}

				} catch(std::invalid_argument &e) {

					cout << "Invalid buffer size: " << optarg << endl;
					cout << "Buffer size must be a positive integer." << endl;
					return EXIT_FAILURE;

				}

				break;

			case 'f':

				try {

					fontSize = std::stod(optarg);

					if(fontSize <= 0) {

						throw std::invalid_argument("Font size must be a positive number.");

					}

				} catch(std::invalid_argument &e) {

					cout << "Invalid font size: " << optarg << endl;
					cout << "Font size must be a positive number." << endl;
					return EXIT_FAILURE;

				}
				break;

			case 'h':

				printUsage();
				return EXIT_SUCCESS;

			default:
 
				printUsage();
				return EXIT_FAILURE;

		}

	}

    // NOTE: This appears to populate the global gApplication and gSystem 
    //       variables.
    TApplication app("DAQManager", nullptr, nullptr);

    // Read in DAQState from file
    State::DAQState state = State::DAQState::getState();
    state.load(STATE_STORAGE);
    state.setSaveFile(STATE_STORAGE);
    state.commit();

    // THIS MUST BE CALLED BEFORE STARTING ANY THREADS.
    // It intercepts SIGINT/SIGTERM/SIGQUIT to cleanly terminate threads.
    setTerminationHandlers(termHandler);

	// Set up GUI styles
	
	// Set graph title fonts
	// The font code is (font number)*10 + precision
	// We want font 6 (the default) and precision 3 (so that sizes are
	// set in pixels instead of as a percentage of pad size), so that's
	// font code 43.
	gStyle->SetTitleFont(63, "t");
	gStyle->SetTitleFontSize(fontSize);

	gStyle->SetLabelFont(63, "XYZ");
	gStyle->SetLabelSize(fontSize, "XYZ");
	gStyle->SetTitleFont(63, "XYZ");
	gStyle->SetTitleSize(fontSize, "XYZ");
	gStyle->SetTitleOffset(0.7, "X");
	
	// We can't set the stat box size absolutely, so we have to set it and
	// the font size of its contents relatively. We signal this by setting
	// a font code with precision 2, then setting the font size by scaling
	// the absolute font size to numbers that I trialed and found looked okay
	// for the default GUI size. This is a workaround to deal with the
	// seeming absence of any way to set the stat box size in absolute terms.
	gStyle->SetStatFont(62);
	gStyle->SetStatFontSize(fontSize * 0.006);
	gStyle->SetStatW(fontSize * 0.036);
	gStyle->SetStatH(fontSize * 0.0175);

    ///////////////////////////////////////////////////////////////////////////
    //////////////////////////// SET UP UI ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Start the UI event loop thread
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(

        thread([pcapBufferSize]() {

            // Create the GUI for the online monitor
            DAQManager *mainFrame = buildGUI();

            // Wire up the main GUI element
            /* NOTE: Unfortunately, we need to define this function alongside
             *       the signal handlers in order to make ROOT be able to find
             *       the definitions for the slots. It's not ideal -- I'd
             *       rather set up the connections in this file.
             */
            connectDAQto(mainFrame);
			setPCapBufferSize(pcapBufferSize);

            // Populate the device selector with connected devices.
            vector<string> devices;
            for(const PCapDevice &device : DataCapture::getNetworkDevices()) {

                devices.push_back(device.name);

            }
            mainFrame->setDeviceSelectorOptions(devices);

            // Load in persistent settings
            State::DAQState state = State::DAQState::getState();

            if(state.persistentState.dataSource == State::DAT_FILE_SOURCE) {

                mainFrame->setFileDataSource();

            } else if(state.persistentState.dataSource == State::NETWORK_DEVICE_SOURCE) {

                mainFrame->setDeviceDataSource();

            } else {

                // (DEFAULT)
                mainFrame->setDeviceDataSource();

            }

            mainFrame->setDeviceSelectorEntry(
                state.persistentState.inputDevicename
            );
            mainFrame->setFileSelectorEntry(
                state.persistentState.inputFilename
            );
            mainFrame->setConfFileSelectorEntry(
                state.persistentState.confFilename
            );

            // Start the UI event loop in order to process user interactions
            // with the GUI
            // NOTE: This will block its calling thread until the loop is
            //       explicitly stopped.
            // TODO: It's not semantically clear that startUILoop blocks its
            //       calling thread until the loop is stopped.
            // TODO: Consider using gApplication->Run() instead -- this is 
            //       ROOT's built-in event loop. I'm worried about thread
            //       safety though.
            //         -- Also look at TApplication::Run(), which we can call
            //            with app.run().
            startUILoop(GUI_REFRESH_RATE, DATA_REFRESH_RATE);

        })

    );
    ProgramFlow::threadLock.unlock();

    ///////////////////////////////////////////////////////////////////////////
    ////////////////////////////// CLEANUP ////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Wait for all threads to be done before terminating
    ProgramFlow::joinAllThreads();
    
    // Save DAQState to file
    state.update();
    state.save  ();

    cout << "Shutdown" << endl;

    // This makes sure the GUI closes when the main thread terminates
    gApplication->Terminate(0);

    return 0;

}