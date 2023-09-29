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
#include <unistd.h>

#include "macros/Monitor.cpp"

#include "src/Geometry.cpp"

#include "monitorConfig.cpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// INTERFACE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: Replace singletons with globals where it makes sense to do so

/**
 * Macro defining the entry command for the monitor.
 */
void StartMonitor(const string &filename);

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// IMPLEMENTATION ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void StartMonitor(const string &filename) {

	Geometry::getInstance()->SetRunN(getRunNumber());

	Monitor monitor(filename);

	while(true) {

		monitor.refresh();

		// TODO: I could consider maintaining a consistent refresh rate by
		//       sleeping until certain clock times
		sleep(1 / REFRESH_RATE);

	}

}