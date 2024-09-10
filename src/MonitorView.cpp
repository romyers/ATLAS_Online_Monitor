#include "MonitorView.h"

using namespace DAQ;

ClassImp(MonitorView);

// For a primer on GUI programming in ROOT, see:
// https://root.cern.ch/root/htmldoc/guides/users-guide/WritingGUI.html

MonitorView::MonitorView(
	SettingsManager &settings,
	const TGWindow *p
) : TGVerticalFrame(p) {

	// This tells ROOT to clean up child frames when the MonitorView is closed.
	SetCleanup(kDeepCleanup);

	// TODO: Use dynamic values for widths and heights
	//         -- Maybe it's as simple as resizing to GetDefaultSize()

	// Whenever we add a UI element to another UI element, we have to:
	//   1. Create it, with the parent element as the first argument
	//   2. Add it to the parent and provide layout hints

	// Layout hints influence the size and position of the element within
	// the parent. We first specify a hint that influences the element's
	// size and position, then specify padding around the element.
	
	// The hint part of layout hints can be combined with bitwise OR. For
	// example, kLHintsExpandX | kLHintsExpandY will make the element expand
	// to fill the parent in both the X and Y directions.

	mainPanel = new TGHorizontalFrame(this);
	AddFrame(mainPanel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

		leftPanel = new TGVerticalFrame(mainPanel, 430, 300, kRaisedFrame);
		mainPanel->AddFrame(
			leftPanel, 
			new TGLayoutHints(kLHintsExpandY | kLHintsLeft)
		);
		leftPanel->ChangeOptions(leftPanel->GetOptions() | kFixedWidth);

			runView = new RunView(leftPanel);
			leftPanel->AddFrame(
				runView, 
				new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5)
			);

			settingsPanel = new SettingsPanel(settings, leftPanel, "Settings");
			leftPanel->AddFrame(
				settingsPanel, 
				new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5)
			);

		tabPanel = new TabPanel(mainPanel, 1250, 800);
		mainPanel->AddFrame(
			tabPanel, 
			new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY)
		);

	bottomPanel = new TGHorizontalFrame(this);
	AddFrame(bottomPanel, new TGLayoutHints(kLHintsExpandX));

		runStatus = new RunStatusPanel(
			bottomPanel
		);
		bottomPanel->AddFrame(
			runStatus, 
			new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 30, 5, 5, 5)
		);

		controlPanel = new ControlPanel(
			bottomPanel, 
			"", 
			kHorizontalFrame, 
			RIGHT
		);
		bottomPanel->AddFrame(
			controlPanel, 
			new TGLayoutHints(kLHintsRight, 5, 50, 5, 5)
		);

}

MonitorView::~MonitorView() {

}