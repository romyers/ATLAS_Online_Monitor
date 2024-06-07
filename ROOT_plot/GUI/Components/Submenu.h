/**
 * @file Submenu.h
 *
 * @brief GUI element describing a menu bar submenu.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "TGMenu.h"

class Submenu : public TGPopupMenu {

ClassDef(Submenu, 0);

public:

	Submenu(const TGWindow *p);

	/**
	 * Adds an entry with an associated behavior to the Submenu. When the entry
	 * is selected, the handler will be called.
	 * 
	 * @param label The label for the entry to be added.
	 * 
	 * @param handler The functional handler defining what should happen when
	 * the entry is selected. Takes the ID and TGMenuEntry corresponding to
	 * the selected entry.
	 * 
	 * @throws std::logic_error if an entry associated with 'label' already 
	 * exists.
	 */
	virtual void AddEntry(
		const std::string &label, 
		std::function<void(int)> handler
	);

	/**
	 * Adds another submenu to the menu, and returns the new submenu.
	 * 
	 * @param label The label for the new submenu as it appears in the old
	 * submenu.
	 * 
	 * @return The new submenu.
	 * 
	 * @throws std::invalid_argument if a submenu associated with 'label' 
	 * already exists.
	 * 
	 * NOTE: Duplicate labels are not allowed.
	 */
	virtual Submenu *AddSubmenu(const std::string &label);

	/**
	 * Retrieves the submenu described by label.
	 * 
	 * @param label The label of the submenu to retrieve.
	 * 
	 * @return The submenu object, or nullptr if it does not exist.
	 */
	virtual Submenu *GetSubmenu(const std::string &label);

	/**
	 * Internal use only.
	 */
	virtual void handleEntrySelect(uint64_t id);

	/**
	 * Internal use only.
	 */
	virtual void makeConnections();

    /**
     * Removes all entries from the menu. Does not affect submenus.
     * 
     * NOTE: A better implementation would clear andremove submenus too. At 
     *       present, this is unnecessary, and it's not obvious how to do it, 
     *       so I've left it out.
     */
    virtual void clear();

    /**
     * Removes an entry from the menu.
     * 
     * @param label The label of the entry to remove.
     */
    virtual void removeEntry(const std::string &label);

private:

    std::map<uint64_t, std::function<void(int)>> entryHandlers;

	std::map<std::string, Submenu*> submenus;

	// Nested submenus must maintain a copy of the top menu for signal handling
	Submenu *topMenu;

	void setTopMenu(Submenu *menu);

};