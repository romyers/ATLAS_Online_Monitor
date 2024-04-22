/**
 * @file TabPanel.h
 *
 * @brief Wrapper for TGTab that better handles associations with menubar
 * options.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGTab.h"
#include "TGFrame.h"
#include "TGLabel.h"

#include "Submenu.h"

class TabPanel : public TGTab {

public:

	TabPanel(const TGWindow *p);

	/**
	 * Associates the tab panel with tabMenu. tabMenu will be populated with
	 * menu options that open and close tabs in the tab panel. 
	 * 
	 * REQUIRES: tabMenu is an empty Submenu.
	 */
	void AttachToMenu(Submenu *tabMenu);

private:

	// A landing page opened when the program starts
	TGCompositeFrame *baseTab;

		TGLabel *baseLabel;

};