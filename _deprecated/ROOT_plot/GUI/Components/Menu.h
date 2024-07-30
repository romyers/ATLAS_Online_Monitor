/**
 * @file Menu.h
 *
 * @brief GUI menu bar element.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <map>
#include <string>

#include "TGMenu.h"

#include "Submenu.h"

class Menu : public TGMenuBar {

public:

	Menu(const TGWindow *p);

	/**
	 * Adds a submenu to the menu bar, and returns the new submenu.
	 * 
	 * @param label The label for the new submenu.
	 * 
	 * @return The new submenu.
	 * 
	 * @throws std::invalid_argument if a submenu associated with 'label' 
	 * already exists.
	 */
	virtual Submenu *AddSubmenu(const std::string &label);

private:

	std::map<std::string, Submenu*> submenus;

};