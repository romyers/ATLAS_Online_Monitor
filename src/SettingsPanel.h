/**
 * @file SettingsPanel.h
 * 
 * @brief Provides a UI element for configuring the settings for a run.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "FileSelector.h"
#include "SettingsManager.h"

#include "TGFrame.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TGComboBox.h"

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
			SettingsManager &settings,
			const TGWindow *p = nullptr,
			const TString &title = "Settings",
			UInt_t options = kVerticalFrame
		);
		virtual ~SettingsPanel();

		/**
		 * @brief Enables the SettingsPanel for user interaction, allowing the
		 * user to change settings.
		 */
		void enable();

		/**
		 * @brief Disables the SettingsPanel, preventing the user from changing
		 * settings.
		 */
		void disable();

		void handleSourceTypeSelection(Int_t button);

		void handleRunNumberChange();

		// TODO: Handlers for device selection and conf/dat file selection

	private:

		SettingsManager &settings;

		TGVerticalFrame *runNumberPanel;

			TGLabel *runNumberEntryLabel;
			
			TGNumberEntry *runNumberEntry;

		FileSelector *confFileSelector;

		TGButtonGroup *sourceTypePanel;

			TGRadioButton *fileButton;
			FileSelector  *datFileSelector;

			TGRadioButton *networkButton;

			TGVerticalFrame *devicePanel;
			
				TGLabel       *deviceLabel;
				TGComboBox    *deviceDropdown;

	};

}