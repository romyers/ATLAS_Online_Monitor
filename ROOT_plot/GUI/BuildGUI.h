/**
 * @file StartDAQ.cpp
 *
 * @brief Entry point for the Phase 2 MiniDAQ online monitor.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"

/**
 * Creates the graphical user interface component for the online monitor.
 */
TGMainFrame *buildGUI();

/**
 * Starts the UI loop.
 */
void startUILoop(double refreshRate);

// TODO: Putting these here is a temporary solution. Fix.
namespace SigHandlers {

	void handlePressedStart();
	void handlePressedStop ();
	void handleExit        ();

}