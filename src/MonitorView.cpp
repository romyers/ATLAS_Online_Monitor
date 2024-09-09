#include "MonitorView.h"

using namespace DAQ;

ClassImp(MonitorView);

MonitorView::MonitorView(
	const TGWindow *p
) : TGVerticalFrame(p) {

	// TODO: Use dynamic values for widths and heights
	//         -- Maybe it's as simple as resizing to GetDefaultSize()

	menuBar = new TGMenuBar(this);
	AddFrame(menuBar, new TGLayoutHints(kLHintsExpandX));

	mainPanel = new TGHorizontalFrame(this);
	AddFrame(mainPanel, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

		leftPanel = new TGVerticalFrame(mainPanel, 430, 300, kRaisedFrame);
		mainPanel->AddFrame(
			leftPanel, 
			new TGLayoutHints(kLHintsExpandY | kLHintsLeft)
		);
		leftPanel->ChangeOptions(leftPanel->GetOptions() | kFixedWidth);

			runView = new RunView(leftPanel);
			leftPanel->AddFrame(
				runView, 
				new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5)
			);

			settings = new SettingsPanel(leftPanel, "Settings");
			leftPanel->AddFrame(
				settings, 
				new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5)
			);

		tabPanel = new TabPanel(mainPanel, 1250, 800);
		mainPanel->AddFrame(
			tabPanel, 
			new TGLayoutHints(kLHintsRight)
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