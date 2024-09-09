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

// TODO: Docs

namespace DAQ {

	class TabPanel : public TGTab {

	ClassDef(TabPanel, 0);

	public:

		TabPanel(
			const TGWindow *p = nullptr,
			int width = 1250,
			int height = 850
		);
		virtual ~TabPanel();

	private:

		TGCompositeFrame *baseTab;
			TGLabel *baseLabel;

	};

}