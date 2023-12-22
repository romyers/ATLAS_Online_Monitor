/**
 * @file SignalHandlers.h
 *
 * @brief Defines and connects signal handlers, knitting together the GUI and
 * data acquisition systems.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"

void connectDAQto(TGMainFrame *GUI);

namespace Muon{
namespace SigHandlers {

	void handleStartRun();
	void handleStopRun ();
	void handleExit    ();

	void handleSelectedFileSource  ();
	void handleSelectedDeviceSource();

	void handleSelectedDevice(const char* selection);
	void handleSelectedFile  (const char* selection);

}
}