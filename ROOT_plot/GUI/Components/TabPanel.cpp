#include "TabPanel.h"

// TODO: Try to get rid of this dependency?
#include "DAQMonitor/DataModel/DAQData.h"

#include <iostream>

using namespace std;

// TODO: Allow setting tab frame sizes in one place. E.g. in the constructor

TabPanel::TabPanel(const TGWindow *p) 
	: TGTab(p) {

    // TODO: Base tab can be expanded into an info panel of some sort -- like a 
    //       readme or like the info screens that pop up sometimes talking about e.g.
    //       program title/version/usage
	baseTab = new TGCompositeFrame(this, 1250, 850, kFixedSize);
	AddTab("Home", baseTab);

		baseLabel = new TGLabel(baseTab, "Open tabs from the view menu");
		baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

}

// Add new tabs here
void TabPanel::AttachToMenu(Submenu *tabMenu) {

	DAQData &data = DAQData::getInstance();

	///////////////////////////////////////////////////////////////////////////
	// ADC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *adcMenu = tabMenu->AddSubmenu("ADC Plots");

		adcMenu->AddEntry("ADC Overview", [](int id) {

			cout << "Pressed ADC Overview" << endl;

		});

		adcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_adc_time.size(); ++i) {

			adcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[i](int id) {

					cout << "Pressed ADC Channel Plots for TDC " << i << endl;

				}
			);

		}

	///////////////////////////////////////////////////////////////////////////
	// TDC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *tdcMenu = tabMenu->AddSubmenu("TDC Plots");

		tdcMenu->AddEntry("TDC Overview", [](int id) {

			cout << "Pressed TDC Overview" << endl;

		});

		tdcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_tdc_time_corrected.size(); ++i) {

			tdcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[i](int id) {

					cout << "Pressed TDC Channel Plots for TDC " << i << endl;

				}
			);

		}

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Noise Rate Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Noise Rate", [](int id) {

		cout << "Pressed Noise Rate" << endl;

	});

	///////////////////////////////////////////////////////////////////////////
	// Error Log Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Error Log", [](int id) {

		cout << "Pressed Error Log" << endl;

	});

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// "Close all" option
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Close All", [](int id) {

		cout << "Pressed close all" << endl;

		// TODO: Close all tabs except home tab and uncheck all menu entries

	});

}