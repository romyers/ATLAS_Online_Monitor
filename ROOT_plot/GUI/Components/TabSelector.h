/**
 * @file TabSelector.h
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

class TabSelector : public TGPopupMenu {

ClassDef(TabSelector, 0);

public:

	TabSelector(const TGWindow *p);

	void handleActivate(int id);

	virtual void AddPopup(
		const char *s, 
		TGPopupMenu *popup, 
		TGMenuEntry *before = nullptr, 
		const TGPicture *p = nullptr
	) override;

    void onSelected(const char *selection); // *SIGNAL*

private:

	// SUBVIEWS

	std::vector<TGPopupMenu*> submenus;

	// VIEW

	TGPopupMenu *adcPlotSelector;

	TGPopupMenu *tdcPlotSelector;

	// CONNECTIONS 

	void makeConnections();

};