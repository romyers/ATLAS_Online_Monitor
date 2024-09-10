/**
 * @file TabPanel.h
 * 
 * @brief Provides a UI element for displaying multiple panels in a tabbed
 * view.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGTab.h"
#include "TGLabel.h"
#include "TGListTree.h"

// TODO: Docs

// TODO: We need the tab menu. I like three ideas:
//         1. Provide a function that allows client code to get a submenu
//            from the TabPanel class and add it wherever they want
//         2. Have a '+' tab that the user can press to add a new tab.
//         3. A vscode style menubar on the right that lists all the tabs
//              -- Consider using a TGListTree for this
// We also want the tab menu not to have to know about the tabs so that we
// can add new tabs easily. A list tree might help here -- we can register
// a new tab with a parent node on the list tree.

// So for a '+' tab, I think when the user selects it, we want to automatically
// switch back to the last tab and open a context menu.

namespace DAQ {

	class TabPanel : public TGHorizontalFrame {

	ClassDef(TabPanel, 0);

	public:

		TabPanel(
			const TGWindow *p = nullptr,
			int width = 1250,
			int height = 850
		);
		virtual ~TabPanel();

		void handleListClick(TGListTreeItem *item);
		void handleListSelection(TGListTreeItem *item);

	private:

		TGTab *tabView;

			TGCompositeFrame *baseTab;
			
				TGLabel *baseLabel;

		TGVerticalFrame *tabSelector;

			TGCanvas *tabCanvas;

				TGListTree *tabList;

	};

}