/**
 * @file RunView.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Views/Components/WindowSelector.cpp"
#include "DAQMonitor/Views/Components/RunStats.cpp"

using namespace std;

class RunView : public TGMainFrame {

public:

	RunView(const TGWindow *p);

private:

	// VIEW

	RunStats       *runStats      ;
	WindowSelector *windowSelector;

	// CONNECTIONS

	void makeConnections();

};

void RunView::makeConnections() {

	// TODO: Before we reimplement this, we need to handle the case where 
	//       RunView stops the run while something else is stopping it or
	//       after something else has already stopped it.
	// Connect("CloseWindow()", "DataRun", nullptr, "stopRun()");
	Connect("CloseWindow()", "RunStats", runStats, "teardown()");

}

RunView::RunView(const TGWindow *p) 
	: TGMainFrame(p) {

	runStats = new RunStats(this);
	AddFrame(
		runStats, new TGLayoutHints(
			kLHintsExpandX | kLHintsExpandY
		)
	);

	windowSelector = new WindowSelector(this, "Select Windows");
	AddFrame(windowSelector, new TGLayoutHints(kLHintsCenterX));

	makeConnections();

}