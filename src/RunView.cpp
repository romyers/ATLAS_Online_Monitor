#include "RunView.h"

using namespace DAQ;

RunView::RunView(
	const TGWindow *p,
	const TString &title
) : TGGroupFrame(p, title) {

	table = new TGHtml(this, 100, 220);
	AddFrame(table, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	Resize(320, 250);

}

RunView::~RunView() {

}

RunStatusPanel::RunStatusPanel(
	const TGWindow *p,
	const TString &defaultLabel,
	UInt_t options
) : TGHorizontalFrame(p, 1, 1, options) {

	runLabel = new TGLabel(this, defaultLabel);
	AddFrame(runLabel, new TGLayoutHints(kLHintsExpandX));

}

RunStatusPanel::~RunStatusPanel() {

}