#include "RunControl.h"

using namespace DAQ;

ClassImp(ControlPanel);

ControlPanel::ControlPanel(
	const TGWindow *p,
	const TString &title,
	UInt_t options,
	int alignment
) : TGButtonGroup(p, title, options) {

	int layoutHints = 0;

	if (alignment == RIGHT) {
		layoutHints = kLHintsRight;
	} else if (alignment == LEFT) {
		layoutHints = kLHintsLeft;
	}

	startButton = new TGTextButton(this, "Start Run");
	AddFrame(startButton, new TGLayoutHints(layoutHints));
	startButton->Connect("Clicked()", "DAQ::ControlPanel", this, "handlePressedStart()");

	stopButton = new TGTextButton(this, "Stop Run");
	AddFrame(stopButton, new TGLayoutHints(layoutHints));
	stopButton ->Connect("Clicked()", "DAQ::ControlPanel", this, "handlePressedStop()" );

}

ControlPanel::~ControlPanel() {

}

void ControlPanel::handlePressedStart() {

	// TODO: Disable settings panel (and reenable if the run fails to start)
	//         -- use a signal for this

	// TODO: Don't enable the stop button until the run has fully started
	startButton->SetEnabled(kFALSE);
	stopButton->SetEnabled(kTRUE);

	throw std::runtime_error("Start Run not yet implemented");

}

void ControlPanel::handlePressedStop() {

	// TODO: Enable settings panel

	// TODO: Don't enable the start button until the run has fully stopped
	startButton->SetEnabled(kTRUE);
	stopButton->SetEnabled(kFALSE);

	throw std::runtime_error("Stop Run not yet implemented");

}