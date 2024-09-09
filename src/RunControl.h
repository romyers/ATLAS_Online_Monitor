/**
 * @file RunView.h
 * 
 * @brief UI interfaces for data runs.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Docs

#pragma once

#include "TGButtonGroup.h"
#include "TGButton.h"

namespace DAQ {

	/**
	 * @brief When passed into the 'alignment' parameter of a ControlPanel,
	 * this indicates that the buttons should be aligned to the right.
	 */
	const int RIGHT = 0;

	/**
	 * @brief When passed into the 'alignment' parameter of a ControlPanel,
	 * this indicates that the buttons should be aligned to the left.
	 */
	const int LEFT = 1;

	/**
	 * @brief A UI element providing buttons for controlling data runs.
	 */
	class ControlPanel : public TGButtonGroup {

	ClassDef(ControlPanel, 0);

	public:

		/**
		 * @brief Construct a new ControlPanel object.
		 * 
		 * @param p The parent window for this ControlPanel.
		 * 
		 * @param title If nonempty, a border will be displayed around the
		 * ControlPanel with this title. By default, no border or title
		 * will be displayed.
		 * 
		 * @param options Options for the ControlPanel, behaving the same as
		 * options for a TGButtonGroup. By default, the ControlPanel will be
		 * configured to be a horizontal frame.
		 */
		ControlPanel(
			const TGWindow *p = nullptr,
			const TString &title = "",
			UInt_t options = kHorizontalFrame,
			int alignment = RIGHT
		);
		virtual ~ControlPanel();

		// Internal signal handlers. They aren't meant to be called directly,
		// so they don't get doxygen comments.
		// This one is called when the "Start Run" button is pressed.
		virtual void handlePressedStart();
		// This one is called when the "Stop Run" button is pressed.
		virtual void handlePressedStop();

	private:

		TGTextButton *startButton;

		TGTextButton *stopButton;

	};

}
