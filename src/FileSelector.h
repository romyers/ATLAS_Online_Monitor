/**
 * @file FileSelector.h
 * 
 * @brief GUI element for file selection.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGFileBrowser.h"
#include "TGTextEntry.h"
#include "TGLabel.h"
#include "TGButton.h"

namespace DAQ {

	/**
	 * @brief A file selector GUI element.
	 * 
	 * This class provides a GUI element that allows the user to select a file
	 * from the filesystem.
	 */
	class FileSelector : public TGVerticalFrame {

	ClassDef(FileSelector, 0);

	public:

		FileSelector(
			const TGWindow *p = nullptr,
			const TString &label = "",
			const TString &defaultEntry = ""
		);
		~FileSelector();

		/**
		 * @brief Configures whether the file selector should allow the user
		 * to select a directory instead of a file.
		 * 
		 * @param allow True if the user should be allowed to select a
		 * directory, false otherwise.
		 */
		void allowDirectorySelections(bool allow);

		/**
		 * @brief Closes the file browser associated with this file selector,
		 * if it is open. If the browser is not open, this method does nothing.
		 */
		void closeBrowser();

		/**
		 * @brief Enables the file selector for user interaction.
		 */
		void enable();
		
		/**
		 * @brief Disables the file selector, preventing the user from selecting
		 * a file.
		 */
		void disable();

		// Signal handlers -- internal use only
		// This one tells the selector what to do if the file button is clicked
		void handleFileButton();
		// This one tells the selector what to do if a file is selected from
		// the browser
		void handleBrowserSelection(TGListTreeItem *selection);

	private:

		bool directoriesAllowed = false;

		TGLabel *labelField;

		TGHorizontalFrame *inputPanel;

			TGTextEntry *entryField;

			TGPictureButton *fileButton;

		TGFileBrowser *fileExplorer;

	};

}