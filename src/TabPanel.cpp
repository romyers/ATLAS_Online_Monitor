#include "TabPanel.h"

using namespace DAQ;

ClassImp(TabPanel);

TabPanel::TabPanel(
	const TGWindow *p,
	int width,
	int height
) : TGHorizontalFrame(p, width, height) {

	tabView = new TGTab(this, width, height);
	AddFrame(tabView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

		baseTab = new TGCompositeFrame(this, width, height);
		tabView->AddTab("Home", baseTab);

			baseLabel = new TGLabel(baseTab, "Open tabs from the View menu");
			baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

		tabView->Resize(width, height);

	tabSelector = new TGVerticalFrame(this);
	AddFrame(tabSelector, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 0, 0, 20, 0));

		tabCanvas = new TGCanvas(tabSelector, 200, height + 20);
		tabSelector->AddFrame(
			tabCanvas, 
			new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)
		);

		tabList = new TGListTree(tabCanvas, kHorizontalFrame);

		// Let's make sure we can open a tab group with just one click
		tabList->Connect(
			"Clicked(TGListTreeItem*, Int_t)", 
			"DAQ::TabPanel", 
			this, 
			"handleListClick(TGListTreeItem*)"
		);
		
		// And let's wire up the tabList to the selection handler
		tabList->Connect(
			"DoubleClicked(TGListTreeItem*, Int_t)", 
			"DAQ::TabPanel", 
			this, 
			"handleListSelection(TGListTreeItem*)"
		);

		
	// Let's set up some icons, sourced from the icons packaged with ROOT
	const TGPicture *tree_node_open = gClient->GetPicture("icons/arrow_down.xpm");
	const TGPicture *tree_node_closed = gClient->GetPicture("icons/arrow_right.xpm");

	// This one I made with GIMP
	const TGPicture *leaf_node = gClient->GetPicture("icons/empty.xpm");

	// TODO: We'll want to not hardcode these
	TGListTreeItem *ADCList = tabList->AddItem(nullptr, "ADC Plots", tree_node_open, tree_node_closed);
	tabList->AddItem(ADCList, "ADC Overview", leaf_node, leaf_node);

	TGListTreeItem *TDCList = tabList->AddItem(nullptr, "TDC Plots", tree_node_open, tree_node_closed);
	tabList->AddItem(TDCList, "TDC Overview", leaf_node, leaf_node);

	tabList->AddItem(nullptr, "Noise Rate", leaf_node, leaf_node);

	tabList->AddItem(nullptr, "Error Log", leaf_node, leaf_node);

	// TODO: How to do a 'close all' button?

	// Options are stored bitwise in an integer type. If we want to
	// change just one option, we want to bitwise OR the new option to the
	// existing options. If we just set the new option, we will overwrite
	// all the existing options.
	// tabView->ChangeOptions(GetOptions() | kFixedSize);

}

TabPanel::~TabPanel() {

}

void TabPanel::handleListClick(TGListTreeItem *item) {

	if(item->IsOpen()) {
		tabList->CloseItem(item);
	} else {
		tabList->OpenItem(item);
	}

}

void TabPanel::handleListSelection(TGListTreeItem *item) {

	throw std::runtime_error("TabList selections not yet implemented");

}