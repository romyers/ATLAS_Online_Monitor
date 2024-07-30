#include "Menu.h"

using namespace std;

Menu::Menu(const TGWindow *p) : TGMenuBar(p, 100, 20, kHorizontalFrame) {

}

Submenu *Menu::AddSubmenu(const string &label) {

	if(
        submenus.find(label) != submenus.end() || 
        submenus.find(string("&") + label) != submenus.end()
    ) {

		throw invalid_argument(
			string("Menu::AddSubmenu() -- Submenu with label ") 
				+ label 
				+ string(" already exists.")
		);

	}

	Submenu *submenu = new Submenu(gClient->GetRoot());
	AddPopup(
		label.data(), 
		submenu, 
		new TGLayoutHints(kLHintsTop | kLHintsLeft)
	);

	submenus[label] = submenu;

	return submenu;

}