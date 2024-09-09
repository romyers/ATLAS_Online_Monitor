/**
 * @file SettingsPanel.h
 * 
 * @brief Provides a UI element for configuring the settings for a run.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"

namespace DAQ {

	/**
	 * @brief A UI element for configuring the settings for a run.
	 * It's a UI frame with a border and title, along with input fields
	 * for each setting.
	 */
	class SettingsPanel : public TGGroupFrame {

	ClassDef(SettingsPanel, 0);

	public:

		/**
		 * @brief Construct a new SettingsPanel object.
		 * 
		 * @param p The parent window for this SettingsPanel.
		 * @param title The title to display on the SettingsPanel.
		 * @param options Options for the SettingsPanel, behaving the same as
		 * options for a TGGroupFrame. By default, the SettingsPanel will be
		 * configured to be a vertical frame.
		 */
		SettingsPanel(
			const TGWindow *p = nullptr,
			const TString &title = "Settings",
			UInt_t options = kVerticalFrame
		);
		virtual ~SettingsPanel();

	private:

	};

}