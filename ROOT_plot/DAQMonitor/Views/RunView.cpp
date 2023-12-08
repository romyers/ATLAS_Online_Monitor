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

using namespace std;

class RunView : public TGGroupFrame {

public:

	RunView(const TGWindow *p);

	void update();

private:

	// VIEW

	RunStats           *runStats      ;

	// CONNECTIONS

	void makeConnections();

};

void RunView::update() {

	runStats->update();

}

void RunView::makeConnections() {

}

RunView::RunView(const TGWindow *p) 
	: TGGroupFrame(p, "Run Viewer", kVerticalFrame) {

	runStats = new RunStats(this);
	AddFrame(
		runStats, new TGLayoutHints(
			kLHintsExpandX | kLHintsExpandY
		)
	);

	Resize(320, 250);

    runStats->update();

	makeConnections();

}