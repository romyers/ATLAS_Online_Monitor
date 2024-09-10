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
#include "TGMenu.h"

#include "RunControl.h"
#include "RunView.h"
#include "SettingsPanel.h"
#include "TabPanel.h"

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

	private:

		MonitorView(const MonitorView &) = delete;
		MonitorView &operator=(const MonitorView &) = delete;

		// The rest of this class represents the GUI layout. I've used
		// indentation to show the hierarchy of the GUI elements. 
		// The elements themselves are constructed and connected to the
		// MonitorView in the MonitorView constructor.

		TGHorizontalFrame *mainPanel;

			TGVerticalFrame *leftPanel;

				RunView *runView;

				SettingsPanel *settings;

			TabPanel *tabPanel;

		TGHorizontalFrame *bottomPanel;

			RunStatusPanel *runStatus;

			ControlPanel *controlPanel;

	};

}