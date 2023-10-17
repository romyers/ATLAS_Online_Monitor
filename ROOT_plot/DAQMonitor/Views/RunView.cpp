/**
 * @file RunView.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/DataRunOperations.cpp"
#include "DAQMonitor/Views/Components/CanvasSelector.cpp"

using namespace std;

// TODO: When we close the RunView, things get a little bit weird bc the
//       run doesn't stop but we can't reopen the RunView. We should be able
//       to do one or the other.

class RunView : public TGMainFrame {

public:

	RunView(const TGWindow *p);

private:

	// VIEW

	CanvasSelector *canvasSelector;

};

RunView::RunView(const TGWindow *p) : TGMainFrame(p) {

	canvasSelector = new CanvasSelector(this, "Select Plots");
	AddFrame(canvasSelector, new TGLayoutHints(kLHintsCenterX));

}