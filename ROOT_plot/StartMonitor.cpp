/**
 * @file StartMonitor.cpp
 *
 * @brief Entry point for the online monitor.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 * 
 * NOTE: This assumes triggerless mode.
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// DEPENDENCIES /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "macros/Monitor.cpp"
#include "macros/Mutexes.cpp"

#include "src/Geometry.cpp"

#include "monitorConfig.cpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// INTERFACE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Replace singletons with globals where it makes sense to do so

// TODO: We will have to multithread; otherwise waiting for monitor refreshes
//       blocks the UI.

// FIXME: Ctrl+c only kills the main thread, and if we try to join the UIThread
//        the terminal will block, forcing us to restart the terminal

// TODO: Try to clean up the multithreading. Ideally all display logic, 
//       including the threading, should be collocated so that we don't need
//       the ugly global mutex. Then we can just call 'startDisplay' or 
//       similar, which will build the UI and start the UI thread. It can
//       also be responsible for pulling events from the monitor and binning
//       them into histograms so that the monitor doesn't have to know about
//       the binning and plotting.

// TODO: Rethink the semantics of Monitor. We've made our labels as if it's a
//       display element when really it does data decoding. It should be the
//       UI element that does the 'refreshing', for example.

/**
 * Macro defining the entry command for the monitor.
 */
void StartMonitor(const string &filename);

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// IMPLEMENTATION ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void StartMonitor(const string &filename) {

	thread UIThread([]() {

		while(true) {

			UILock.lock();
			gSystem->ProcessEvents();
			UILock.unlock();

			this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

		}

	});

	Geometry::getInstance()->SetRunN(getRunNumber());

	Monitor monitor(filename);

	while(true) {

		monitor.refresh();

		this_thread::sleep_for(chrono::milliseconds((int)(1000 / DATA_REFRESH_RATE)));

	}

	// UIThread.join();

}