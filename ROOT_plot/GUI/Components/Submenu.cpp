#include "Submenu.h"

using namespace std;

ClassImp(Submenu);

Submenu::Submenu(const TGWindow *p) : TGPopupMenu(p), topMenu(this) {

	makeConnections();

}

void Submenu::makeConnections() {

	// TODO: Don't do this for nested submenus?

	Connect(
		"Activated(Int_t)",
		"Submenu",
		this,
		"handleEntrySelect(Int_t)"
	);

}

void Submenu::AddEntry(
	const string &label, 
	std::function<void(int)> handler
) {

	if(GetEntry(label.data())) {

		throw invalid_argument(
			string("Submenu::AddEntry tried to add an entry that already exits.")
				+ string(" Entry: ") 
				+ label
		);

	}

	// NOTE: The entry ID must match the handler's index in the top level
	//       submenu. ROOT refuses to let nested menus process their own events
	//       and calls activated() on the top-level submenu, so we must pass 
	//       the handler up to the top-level submenu and pair it with the ID
	//       assigned to it by that menu.

	//       So we get the entry ID from the top menu, then add the handeler to
	//       the top menu.
	TGPopupMenu::AddEntry(label.data(), topMenu->entryHandlers.size());

	topMenu->entryHandlers.push_back(handler);

}

// Calls the appropriate handler when an entry is selected.
void Submenu::handleEntrySelect(Int_t id) {

	if(id < 0) {

		throw invalid_argument(
			string("Submenu::handleEntrySelect encountered out-of-bounds ID: ")
				+ to_string(id)
		);

	}

	if(id >= topMenu->entryHandlers.size()) {

		throw invalid_argument(
			string("Submenu::handleEntrySelect encountered out-of-bounds ID: ")
				+ to_string(id)
		);

	}

	TGMenuEntry *entry = GetEntry(id);
	
	topMenu->entryHandlers[id](id);

}

Submenu *Submenu::AddSubmenu(const string &label) {

	// TODO: Since ROOT is silly and calls all handlers on the top level
	//       submenu, we need to make sure submenus further down get their
	//       handlers propagated up to the top level.
	/*
	So here's what we do. Each submenu stores its parent. When we add an entry,
	we check for a parent submenu, and if there is one, we push the handler
	up the stack and get the new entry id from the parent submenu.

	Any nested submenus will have empty handler vectors, but eh
	*/

	if(submenus.find(label) != submenus.end()) {

		throw logic_error(
			string("Submenu::AddSubmenu() -- Submenu with label ") 
				+ label 
				+ string(" already exists.")
		);

	}

	Submenu *submenu = new Submenu(gClient->GetRoot());
	AddPopup(label.data(), submenu);

	submenus[label] = submenu;

	// Store the top-level submenu in the new submenu
	submenu->setTopMenu(this->topMenu);

	return submenu;

}

// This just helps us make sure that if this menu has any submenus, their
// top menus get updated too. That way we don't require an order for
// menu construction.
void Submenu::setTopMenu(Submenu *menu) {

	topMenu = menu;

	// Iterate through all existing submenus
	for(auto it = submenus.begin(); it != submenus.end(); ++it) {

		// and update their topMenu too.
		it->second->setTopMenu(menu);

	}

}

Submenu *Submenu::GetSubmenu(const string &label) {

	auto it = submenus.find(label);

	if(it != submenus.end()) {

		return it->second;

	}

	return nullptr;

}