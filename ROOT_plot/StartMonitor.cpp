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

#include "macros/Monitor.cpp"
#include "macros/Display.cpp"

#include "src/Geometry.cpp"

#include <unistd.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// INTERFACE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Replace singletons with globals where it makes sense to do so

// TODO: All config parameters should be in a centralized place
/**
 * The approximate rate at which the monitor is refreshed. Note that the
 * monitor will fall short of this rate if it must process too much data
 * at a time.
 */
const double REFRESH_RATE = 10.; // Hz

/**
 * Macro defining the entry command for the monitor.
 */
void StartMonitor(const string &filename);

/**
 * A function setting the run number. Can be edited to control how runs are
 * distinguished.
 */
inline int getRunNumber() { return 0; }

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// IMPLEMENTATION ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void StartMonitor(const string &filename) {

	Geometry::getInstance()->SetRunN(getRunNumber());

	Monitor monitor(filename);

	while(true) {

		monitor.refresh();

		// TODO: I could consider maintaining a consistent refresh rate by
		//       sleeping until certain clock times or by keeping a clock
		//       in a separate thread that tells us when to refresh. But I can
		//       wait until I have a good reason to do this.
		sleep(1 / REFRESH_RATE);

	}

}