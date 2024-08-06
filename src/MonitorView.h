/**
 * @file MonitorView.h
 * 
 * @brief Top-level GUI interface for the DAQMonitor.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGButtonGroup.h"

#include <string>

namespace DAQ {

	/**
	 * @brief Top-level GUI interface for the DAQMonitor.
	 */
	class MonitorView final : public TGVerticalFrame {

	ClassDef(MonitorView, 0);

	public:

		/**
		 * @brief Construct a new MonitorView object.
		 * 
		 * @param p The window that should serve as this MonitorView's parent.
		 * By default, the parent is the root window.
		 */
		MonitorView(
			const TGWindow *p = gClient->GetRoot()
		);
		virtual ~MonitorView();

		// Internal signal handlers. Not meant to be called directly.
		// This one is called when the "Start Run" button is pressed.
		void handlePressedStart();
		// This one is called when the "Stop Run" button is pressed.
		void handlePressedStop();

	private:

		MonitorView(const MonitorView &) = delete;
		MonitorView &operator=(const MonitorView &) = delete;

		// The rest of this class represents the GUI layout. I've used
		// indentation to show the hierarchy of the GUI elements. 
		// The elements themselves are constructed and connected to the
		// MonitorView in the MonitorView constructor.

		TGHorizontalFrame *mainPanel;

			TGVerticalFrame *leftPanel;

		TGHorizontalFrame *bottomPanel;

			TGButtonGroup *buttonGroup;

				TGTextButton *startButton;
				TGTextButton *stopButton ;

	};

}