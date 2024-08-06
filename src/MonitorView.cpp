#include "MonitorView.h"

using namespace DAQ;

ClassImp(MonitorView);

MonitorView::MonitorView(
	const TGWindow *p
) : TGVerticalFrame(p) {

	// TODO: Use dynamic values for widths and heights
	//         -- Maybe it's as simple as resizing to GetDefaultSize()

	mainPanel = new TGHorizontalFrame(this);
	AddFrame(mainPanel, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

		leftPanel = new TGVerticalFrame(mainPanel, 430, 300, kRaisedFrame);
		mainPanel->AddFrame(leftPanel, new TGLayoutHints(kLHintsExpandY | kLHintsLeft));
		leftPanel->ChangeOptions(leftPanel->GetOptions() | kFixedWidth);

	bottomPanel = new TGHorizontalFrame(this);
	AddFrame(bottomPanel, new TGLayoutHints(kLHintsExpandX));

		buttonGroup = new TGButtonGroup(bottomPanel, "", kHorizontalFrame);
		bottomPanel->AddFrame(buttonGroup, new TGLayoutHints(kLHintsRight, 5, 50, 5, 5));

			startButton = new TGTextButton(buttonGroup, "Start Run");
			buttonGroup->AddFrame(startButton, new TGLayoutHints(kLHintsRight));

			stopButton = new TGTextButton(buttonGroup, "Stop Run");
			buttonGroup->AddFrame(stopButton, new TGLayoutHints(kLHintsRight));

	startButton->Connect("Clicked()", "DAQ::MonitorView", this, "handlePressedStart()");
	stopButton->Connect("Clicked()", "DAQ::MonitorView", this, "handlePressedStop()");

}

MonitorView::~MonitorView() {

}

void MonitorView::handlePressedStart() {

	throw std::runtime_error("Start Run not yet implemented");

}

void MonitorView::handlePressedStop() {

	throw std::runtime_error("Stop Run not yet implemented");

}