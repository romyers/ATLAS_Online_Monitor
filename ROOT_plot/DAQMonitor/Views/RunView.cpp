#include "RunView.h"

using namespace std;

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