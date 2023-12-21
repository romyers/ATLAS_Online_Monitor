/**
 * @file TabSelector.cpp
 *
 * @brief Popup menu providing UI for selecting DAQ monitor tabs.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>

#include "TQObject.h"

#include "TGMenu.h"

class TabSignalBus : public TQObject {

public:

	TabSignalBus();

    TabSignalBus  (      TabSignalBus &other) = delete;
    void operator=(const TabSignalBus &other) = delete;

    void onSelected  (const char *selection); // *SIGNAL*

};

class TabSelector : public TGPopupMenu {

public:

	TabSelector(const TGWindow *p);

	void handleActivate(int id);

	TabSignalBus &getSignalBus();

	virtual void AddPopup(
		const char *s, 
		TGPopupMenu *popup, 
		TGMenuEntry *before = nullptr, 
		const TGPicture *p = nullptr
	) override {

		TGPopupMenu::AddPopup(s, popup, before, p);

		submenus.push_back(popup);

	}

private:

	// SUBVIEWS

	std::vector<TGPopupMenu*> submenus;

	// VIEW

	TGPopupMenu *adcPlotSelector;

	TGPopupMenu *tdcPlotSelector;

	// SIGNALS

	TabSignalBus signals;

	// CONNECTIONS 

	void makeConnections();

};