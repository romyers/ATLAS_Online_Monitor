#include "TabPanel.h"

using namespace DAQ;

ClassImp(TabPanel);

TabPanel::TabPanel(
	const TGWindow *p,
	int width,
	int height
) : TGTab(p, width, height) {

	baseTab = new TGCompositeFrame(this, width, height, kFixedSize);
	AddTab("Home", baseTab);

		baseLabel = new TGLabel(baseTab, "Open tabs from the View menu");
		baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

	Resize(width, height);

	// Options are stored bitwise in an integer type. If we want to
	// change just one option, we want to bitwise OR the new option to the
	// existing options. If we just set the new option, we will overwrite
	// all the existing options.
	// ChangeOptions(GetOptions() | kFixedSize);

}

TabPanel::~TabPanel() {

}