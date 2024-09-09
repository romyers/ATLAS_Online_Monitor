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

	stopButton = new TGTextButton(this, "Stop Run");
	AddFrame(stopButton, new TGLayoutHints(layoutHints));

	startButton->Connect("Clicked()", "DAQ::ControlPanel", this, "handlePressedStart()");
	stopButton ->Connect("Clicked()", "DAQ::ControlPanel", this, "handlePressedStop()" );

}

ControlPanel::~ControlPanel() {

}

void ControlPanel::handlePressedStart() {

	throw std::runtime_error("Start Run not yet implemented");

}

void ControlPanel::handlePressedStop() {

	throw std::runtime_error("Stop Run not yet implemented");

}