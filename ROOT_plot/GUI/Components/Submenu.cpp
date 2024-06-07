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

    // This assigns entryIDs to each entry while avoiding collisions between 
    // child submenus.
    static uint64_t entryID = 0;
    ++entryID;

	// NOTE: The entry ID must match the handler's ID in the top level
	//       submenu. ROOT refuses to let nested menus process their own events
	//       and calls activated() on the top-level submenu, so we must pass 
	//       the handler up to the top-level submenu and pair it with the ID
	//       assigned to it by that menu.

	//       So we have to add the handler to the top menu.
	TGPopupMenu::AddEntry(label.data(), entryID);

    if(topMenu->entryHandlers.count(entryID) > 0) {

        throw logic_error(
            string("Submenu::AddEntry() -- Entry ID collision detected: ")
                + to_string(entryID)
                + string(". This should never happen, so check the ")
                + string("implementation of Submenu::AddEntry() and ")
                + string("Submenu::handleEntrySelect() in Submenu.cpp.")
        );

    }

	topMenu->entryHandlers[entryID] = handler;

}

// Calls the appropriate handler when an entry is selected.
void Submenu::handleEntrySelect(uint64_t id) {

    if(topMenu->entryHandlers.count(id) == 0) {

        throw invalid_argument(
            string("Submenu::handleEntrySelect() -- No handler found for ID: ")
                + to_string(id)
        );

    }
	
	topMenu->entryHandlers[id](id);

}

Submenu *Submenu::AddSubmenu(const string &label) {

	/*
    Since ROOT is silly and calls all handlers on the top level
	submenu, we need to make sure submenus further down get their
	handlers propagated up to the top level.

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

    // Quick optimization for a common case where the top menu is the same.
    if(topMenu == menu) return;

    // FIXME: Ooh, problem -- if we change the top menu, we need to remove handlers
    //        from the old top menu and add them to the new one
    //          -- This kind of issue is why we could really use some unit testing.
    //             It's not an issue that will show up right now though, since the
    //             way AddSubmenu works guarantees that the top menu is invariant,
    //             but we should really fix it at some point.
    //          -- Try to iterate through top menu's handlers, find the ones 
    //             that correspond to local entries, and move them to the new
    //             topMenu.

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

void Submenu::removeEntry(const string &label) {

    TGMenuEntry *entry = GetEntry(label.data());

    if(!entry) {

        throw invalid_argument(
            string("Submenu::removeEntry() -- No entry found with label: ")
                + label
        );

    }

    // Remove the entry's handler from the top menu
    topMenu->entryHandlers.erase(entry->GetEntryId());

    // Remove the entry from the TGPopupMenu
    DeleteEntry(entry);

}

void Submenu::clear() {

    // The issue we have here is that we need to remove handlers from the
    // top menu. But we don't want the top menu's handler vector to keep
    // growing, and we can't remove the handlers from the top menu without
    // disrupting the connection between entry IDs and handlers.
    //   -- So we need to redefine how handlers and entry IDs are connected.
    //      We will have to replace the handler vector with a map of ints
    //      to handlers.

    const TList *entries = GetListOfEntries();

    for(const auto &&entry : *entries) {

        TGMenuEntry *menuEntry = (TGMenuEntry*)entry;

        // Remove the handler from the top menu
        topMenu->entryHandlers.erase(menuEntry->GetEntryId());

        DeleteEntry(menuEntry);

    }

}