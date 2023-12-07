/**
 * @file RunView.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Views/Components/RunStats.cpp"
#include "DAQMonitor/Views/Components/RunMenuBar.cpp"

using namespace std;

class RunView : public TGVerticalFrame {

public:

	RunView(const TGWindow *p);

	void update();

private:

	// VIEW

	RunMenuBar         *menuBar       ;
	RunStats           *runStats      ;

	// CONNECTIONS

	void makeConnections();

};

void RunView::update() {

	runStats->update();

}

void RunView::makeConnections() {
	
	Connect("CloseWindow()", "RunStats", runStats, "teardown()");

}

RunView::RunView(const TGWindow *p) 
	: TGVerticalFrame(p, 1, 1, kFixedSize) {

	menuBar = new RunMenuBar(this);
	AddFrame(menuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft));

	runStats = new RunStats(this);
	AddFrame(
		runStats, new TGLayoutHints(
			kLHintsExpandX | kLHintsExpandY
		)
	);

	Resize(300, 250);

	SetWindowName("Run Viewer");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();

    runStats->update();

	makeConnections();

}