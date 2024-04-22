#include "TabPanel.h"

// TODO: Try to get rid of this dependency?
#include "DAQMonitor/DataModel/DAQData.h"

#include "ErrorView.h"
#include "GraphPlotter.h"
#include "HistogramPlotter.h"

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

	makeConnections();

}

void TabPanel::makeConnections() {

	Connect(
		"CloseTab(Int_t)", 
		"TabPanel", 
		this, 
		"handleCloseTab(int)"
	);

}

// Add new tabs here
void TabPanel::AttachToMenu(Submenu *tabMenu) {

	DAQData &data = DAQData::getInstance();

	///////////////////////////////////////////////////////////////////////////
	// ADC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *adcMenu = tabMenu->AddSubmenu("ADC Plots");

		adcMenu->AddEntry("ADC Overview", [this, &data](int id) {

			if(!GetTabTab("ADC Overview")) {

				vector<TH1*> plotList(
					data.plots.p_tdc_adc_time.begin(),
					data.plots.p_tdc_adc_time.end()
				);

				HistogramPlotter *adcOverview = new HistogramPlotter(
					this,
					plotList,
					"ADC Plots",
					1250,
					850,
					4
				);

				buildTab("ADC Overview", adcOverview);

			}

			// Switch to the new tab
			SetTab("ADC Overview");

		});

		adcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_adc_time.size(); ++i) {

			adcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[this, &data, i](int id) {

					string plotTitle = string("TDC ") 
						+ to_string(i) 
						+ string(" ADC Channels");

					if(!GetTabTab(plotTitle.data())) {

						vector<TH1*> plotList(
							data.plots.p_adc_time[i].begin(),
							data.plots.p_adc_time[i].end()
						);

						HistogramPlotter *adcChannelPlot = new HistogramPlotter(
							this,
							plotList,
							plotTitle,
							1250,
							850,
							4
						);

						buildTab(plotTitle, adcChannelPlot);

					}

					// Switch to the new tab
					SetTab(plotTitle.data());

				}
			);

		}

	///////////////////////////////////////////////////////////////////////////
	// TDC Tabs
	///////////////////////////////////////////////////////////////////////////

	Submenu *tdcMenu = tabMenu->AddSubmenu("TDC Plots");

		tdcMenu->AddEntry("TDC Overview", [this, &data](int id) {

			if(!GetTabTab("TDC Overview")) {

				vector<TH1*> plotList(
					data.plots.p_tdc_tdc_time_corrected.begin(),
					data.plots.p_tdc_tdc_time_corrected.end()
				);

				HistogramPlotter *tdcOverview = new HistogramPlotter(
					this,
					plotList,
					"TDC Plots",
					1250,
					850,
					4
				);

				buildTab("TDC Overview", tdcOverview);

			}

			// Switch to the new tab
			SetTab("TDC Overview");

		});

		tdcMenu->AddSeparator();

		for(int i = 0; i < data.plots.p_tdc_time_corrected.size(); ++i) {

			tdcMenu->AddEntry(
				string("TDC ") + to_string(i),
				[this, &data, i](int id) {

					string plotTitle = string("TDC ") 
						+ to_string(i)
						+ string(" TDC Channels");

					if(!GetTabTab(plotTitle.data())) {

						vector<TH1*> plotList(
							data.plots.p_tdc_time_corrected[i].begin(),
							data.plots.p_tdc_time_corrected[i].end()
						);

						HistogramPlotter *tdcChannelPlot = new HistogramPlotter(
							this,
							plotList,
							plotTitle,
							1250,
							850,
							4
						);

						buildTab(plotTitle, tdcChannelPlot);

					}

					// Switch to the new tab
					SetTab(plotTitle.data());

				}
			);

		}

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Noise Rate Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Noise Rate", [this, &data](int id) {

		if(!GetTabTab("Noise Rate")) {

			GraphPlotter *noiseDisplay = new GraphPlotter(
				this,
				data.plots.p_tdc_hit_rate_graph,
				"Noise Rate Display",
				1250,
				850,
				4
			);

			buildTab("Noise Rate", noiseDisplay);

		}

		// Switch to the new tab
		SetTab("Noise Rate");

	});

	///////////////////////////////////////////////////////////////////////////
	// Error Log Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Error Log", [this](int id) {

		if(!GetTabTab("Error Log")) {

			ErrorView *errorViewer = new ErrorView(this);

			buildTab("Error Log", errorViewer);

		}

		// Switch to the new tab
		SetTab("Error Log");

	});

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// "Close all" option
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Close All", [this](int id) {

		// We don't delete the first one -- that's the HOME tab.
		while(GetNumberOfTabs() > 1) {

			TGCompositeFrame *tab = GetTabContainer(1);

			RemoveTab(1, kFALSE);
			Layout();

			delete tab;

		}

	});

}

void TabPanel::buildTab(const string &label, UITab *tab) {

	AddTab(label.data(), tab);
	MapSubwindows();
	Layout();

	// Show the close button for the tab
	GetTabTab(label.data())->ShowClose();

}

void TabPanel::handleCloseTab(int id) {

	TGCompositeFrame *tab = GetTabContainer(id);

	RemoveTab(id, kFALSE);
	Layout();

	delete tab;

}