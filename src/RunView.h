/**
 * @file RunView.h
 * 
 * @brief Provides a UI element for displaying aggregated data for a run.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGHtml.h"
#include "TGLabel.h"

namespace DAQ {
	
	/**
	 * @brief A UI element for displaying aggregated data for a run. A RunView
	 * will include a table of data describing the total numbers of events,
	 * signals, errors, and other relevant information for a run.
	 */
	class RunView : public TGGroupFrame {

	public:

		/**
		 * @brief Construct a new RunView object.
		 * 
		 * @param p The parent window for this RunView.
		 * @param title The title to display on the RunView. By default, the
		 * title is "Run View".
		 */
		RunView(
			const TGWindow *p = nullptr,
			const TString &title = "Run View"
		);
		virtual ~RunView();

	private:

		TGHtml *table;

	};

	// TODO: Rename this file to better capture that this is the file holding
	//       all UI elements associated with run state.
	class RunStatusPanel : public TGHorizontalFrame {

	public:

		RunStatusPanel(
			const TGWindow *p = nullptr,
			const TString &defaultLabel = "No run in progress",
			UInt_t options = kHorizontalFrame
		);
		virtual ~RunStatusPanel();

	private:

		TGLabel *runLabel;

	};

}