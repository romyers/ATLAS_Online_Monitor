#include "UILoop.h"

#include <chrono>
#include <cmath>
#include <thread>

#include "TSystem.h"

#include "UILock.h"
#include "UIException.h"
#include "UISignals.h"

#include "AlertBox/AlertOperations.h"

#include "DAQMonitor/DataModel/DAQData.h"

using namespace std;

bool isRunning = false;

/**
 * Runs a UI event loop step.
 * 
 * @return The time it took the loop step to complete.
 */
chrono::milliseconds runLoopStep();

/**
 * Runs a data loop step.
 * 
 * @return The time it took the loop step to complete.
 */
chrono::milliseconds runDataLoopStep();

void startLoop(double refreshRate, double dataRefreshRate) {

    if(isRunning) return;

    isRunning = true;

	thread eventLooper([refreshRate](){

		// TODO: Remove condition on terminator, and have something call stopRun()
		//       at a higher level when the terminator terminates. Can do this by
		//       having the terminator be a TQObject and signaling when it 
		//       terminates.
		// Run the UI update loop
		while(isRunning) {

			chrono::milliseconds updateTime = runLoopStep();

			// Compute how long the thread should sleep to maintain a 
			// consistent frame rate
			int sleepTime = (int)(1000. / refreshRate) - (int)(updateTime.count());

			// Deal with the case where the update loop took too long
			if(sleepTime < 0) {

				sleepTime = 0;

			}

			// Sleep until the next update time.
			this_thread::sleep_for(
				chrono::milliseconds(sleepTime)
			);

		}

	});

	thread dataLooper([dataRefreshRate]() {

		// TODO: Remove condition on terminator, and have something call stopRun()
		//       at a higher level when the terminator terminates. Can do this by
		//       having the terminator be a TQObject and signaling when it 
		//       terminates.
		// Run the UI update loop
		// TODO: Pause the data loop when there is no new data to display.
		//       Somehow signal this to the data loop.
		while(isRunning) {

			chrono::milliseconds updateTime = runDataLoopStep();

			// Compute how long the thread should sleep to maintain a 
			// consistent frame rate
			int sleepTime = (int)(1000. / dataRefreshRate) - (int)(updateTime.count());

			// Make sure we sleep at least a little bit
			if(sleepTime < 100) {

				sleepTime = 100;

			}

			// Sleep until the next update time.
			this_thread::sleep_for(
				chrono::milliseconds(sleepTime)
			);

		}

	});

	eventLooper.join();
	dataLooper.join();

}

void stopLoop() {

    isRunning = false;

}

chrono::milliseconds runDataLoopStep() {

    // Record the loop start time
    auto UIUpdateStartTime = chrono::high_resolution_clock::now();

	

	DAQData &data = DAQData::getInstance();

	data.lock();
	bool refreshNeeded = data.isDirty();
	data.markClean();
	data.unlock();

	if(refreshNeeded) {

		// TODO: I don't like the UILock being used at this level.
		// Process UI events.
		UI::UILock.lock();
		try {

			UISignalBus::getInstance().onUpdate();

		} catch (UIException &e) {

			// TODO: I don't want this here.
			AlertPopup::popupAlert(
				gClient->GetRoot(), "Error", e.what()
			);

		}
		UI::UILock.unlock();

	}

    // Record the loop end time
    auto UIUpdateEndTime = chrono::high_resolution_clock::now();

    // Compute how long it took the loop to run
    chrono::duration<double> loopTime = UIUpdateEndTime - UIUpdateStartTime;

    return chrono::duration_cast<chrono::milliseconds>(loopTime);

}

chrono::milliseconds runLoopStep() {

    // Record the loop start time
    auto UIUpdateStartTime = chrono::high_resolution_clock::now();

    // TODO: I don't like the UILock being used at this level.
    // Process UI events.
    UI::UILock.lock();
    try {

        gSystem->ProcessEvents();

    } catch (UIException &e) {

    	// TODO: I don't want this here.
        AlertPopup::popupAlert(
            gClient->GetRoot(), "Error", e.what()
        );

    }
    UI::UILock.unlock();

    // Record the loop end time
    auto UIUpdateEndTime = chrono::high_resolution_clock::now();

    // Compute how long it took the loop to run
    chrono::duration<double> loopTime = UIUpdateEndTime - UIUpdateStartTime;

    return chrono::duration_cast<chrono::milliseconds>(loopTime);

}