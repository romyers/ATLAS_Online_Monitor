#include "TabSelector.h"

using namespace std;

ClassImp(TabSelector);

TabSelector::TabSelector(const TGWindow *p) : TGPopupMenu(p) {

	submenus.push_back(this);

	makeConnections();

}

void TabSelector::makeConnections() {

	Connect("Activated(Int_t)", "TabSelector", this, "handleActivate(int)");

}

void TabSelector::handleActivate(int id) {

	// We want to unify the handling for activation of entries in this menu or
	// in any submenu. So we override AddPopup to store the new popup in a 
	// vector of submenus, and here we just scan all the submenus for the
	// id of the activated entry.
	for(TGPopupMenu *menu : submenus) {

		TGMenuEntry *entry = menu->GetEntry(id);

		if(entry) {

			onSelected(entry->GetName());

			return;

		}

	}

	throw invalid_argument("TabSelector::handleActivate() received nonexistent entry ID.");

}

void TabSelector::AddPopup(
	const char *s, 
	TGPopupMenu *popup, 
	TGMenuEntry *before, 
	const TGPicture *p
) {

	TGPopupMenu::AddPopup(s, popup, before, p);

	submenus.push_back(popup);

}

void TabSelector::onSelected(const char *selection) {

	Emit("onSelected(const char*)", selection);

}