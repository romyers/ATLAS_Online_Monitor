#include "TabPanel.h"

// TODO: Try to get rid of this dependency?
#include "DAQMonitor/DataModel/DAQData.h"

#include "ErrorView.h"
#include "GraphPlotter.h"
#include "HistogramPlotter.h"
#include "EventTab.h"
#include "ResidualsDisplay.h"
#include "EfficiencyDisplay.h"

using namespace std;

// TODO: Allow setting tab frame sizes in one place. E.g. in the constructor

TabPanel::TabPanel(const TGWindow *p, int width, int height, Menu *menuBar)
	: TGTab(p), width(width), height(height), 
    tabMenu(nullptr), 
    adcMenu(nullptr), 
    tdcMenu(nullptr),
    adcChannelPlotCount(0),
    tdcChannelPlotCount(0)
{

    // TODO: Base tab can be expanded into an info panel of some sort -- like a 
    //       readme or like the info screens that pop up sometimes talking about e.g.
    //       program title/version/usage
	baseTab = new TGCompositeFrame(this, width, 850, kFixedSize);
	AddTab("Home", baseTab);

		baseLabel = new TGLabel(baseTab, "Open tabs from the view menu");
		baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

    if(menuBar) AttachToMenu(menuBar);

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
void TabPanel::AttachToMenu(Menu *menuBar) {

    tabMenu = menuBar->AddSubmenu("&View");

	DAQData &data = DAQData::getInstance();

	///////////////////////////////////////////////////////////////////////////
	// ADC Tabs
	///////////////////////////////////////////////////////////////////////////

	adcMenu = tabMenu->AddSubmenu("ADC Plots");

		adcMenu->AddEntry("ADC Overview", [this, &data](int id) {

			if(!hasTab("ADC Overview")) {

				HistogramPlotter *adcOverview = new HistogramPlotter(
					this,
					data.plots.p_tdc_adc_time,
					"ADC Plots",
					width,
					height
				);

				buildTab("ADC Overview", adcOverview);

			}

			// Switch to the new tab
			SetTab("ADC Overview");

		});

		adcMenu->AddSeparator();

        // The update loop will add ADC channel plots here

	///////////////////////////////////////////////////////////////////////////
	// TDC Tabs
	///////////////////////////////////////////////////////////////////////////

	tdcMenu = tabMenu->AddSubmenu("TDC Plots");

		tdcMenu->AddEntry("TDC Overview", [this, &data](int id) {

			if(!hasTab("TDC Overview")) {

				HistogramPlotter *tdcOverview = new HistogramPlotter(
					this,
					data.plots.p_tdc_tdc_time_corrected,
					"TDC Plots",
					width,
					height
				);

				buildTab("TDC Overview", tdcOverview);

			}

			// Switch to the new tab
			SetTab("TDC Overview");

		});

		tdcMenu->AddSeparator();

		// The update loop will add TDC channel plots here

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Event Displays
	///////////////////////////////////////////////////////////////////////////

    tabMenu->AddEntry("Residuals", [this](int id) {

		if(!hasTab("Residuals")) {

			ResidualsDisplay *residuals = new ResidualsDisplay(
				this, 
				width, 
				height
			);

			buildTab("Residuals", residuals);

		}

		SetTab("Residuals");

	});

	tabMenu->AddEntry("Efficiency", [this](int id) {

		if(!hasTab("Efficiency")) {

			EfficiencyDisplay *efficiency = new EfficiencyDisplay(
				this, 
				width, 
				height
			);

			buildTab("Efficiency", efficiency);

		}

		SetTab("Efficiency");

	});

	tabMenu->AddEntry("Event Display", [this](int id) {

		if(!hasTab("Event Display")) {

			EventTab *eventDisplay = new EventTab(this, width, height);

			buildTab("Event Display", eventDisplay);

		}

		SetTab("Event Display");

	});

	tabMenu->AddSeparator();

	///////////////////////////////////////////////////////////////////////////
	// Noise Rate Tab
	///////////////////////////////////////////////////////////////////////////

	tabMenu->AddEntry("Noise Rate", [this, &data](int id) {

		if(!hasTab("Noise Rate")) {

			GraphPlotter *noiseDisplay = new GraphPlotter(
				this,
				data.plots.p_tdc_hit_rate_graph,
				"Noise Rate Display",
				width,
				height
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

		if(!hasTab("Error Log")) {

			ErrorView *errorViewer = new ErrorView(this, width, height);

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

void TabPanel::update() {

    // We don't need to do anything if there's no attached menu.
    if(!isAttached()) return;

    bool relayoutNeeded = false;

    DAQData &data = DAQData::getInstance();

    data.lock();

    while(data.plots.p_adc_time.size() > adcChannelPlotCount) {

        int tdc = adcChannelPlotCount;

        // Add missing entries
        adcMenu->AddEntry(
            string("TDC ") + to_string(adcChannelPlotCount),
            [this, &data, tdc](int id) {

                string plotTitle = string("TDC ") 
                    + to_string(tdc) 
                    + string(" ADC Channels");

                if(!hasTab(plotTitle.data())) {

                    HistogramPlotter *adcChannelPlot = new HistogramPlotter(
                        this,
                        data.plots.p_adc_time[tdc],
                        plotTitle,
                        width,
                        height
                    );

                    buildTab(plotTitle, adcChannelPlot);

                }

                // Switch to the new tab
                SetTab(plotTitle.data());

            }
        );

        ++adcChannelPlotCount;

    }

    while(data.plots.p_adc_time.size() < adcChannelPlotCount) {

        // Remove extra entries and close associated tabs

        int tdc = adcChannelPlotCount - 1;

        adcMenu->removeEntry(string("TDC ") + to_string(tdc));

        string plotTitle = string("TDC ") 
            + to_string(tdc)
            + string(" ADC Channels");

        // We need to get the tab index

        int tabIndex = -1;
        for(int i = 0; i < GetNumberOfTabs(); ++i) {

            if(strcmp(GetTabTab(i)->GetString(), plotTitle.data()) == 0) {

                tabIndex = i;
                break;

            }

        }
        
        if(tabIndex != -1) {
                
            TGCompositeFrame *tab = GetTabContainer(tabIndex);

            RemoveTab(tabIndex, kFALSE);

            delete tab;

            relayoutNeeded = true;

        }

        --adcChannelPlotCount;

    }

    while(data.plots.p_tdc_time_corrected.size() > tdcChannelPlotCount) {

        int tdc = tdcChannelPlotCount;
        
        // Add missing entries
        tdcMenu->AddEntry(
            string("TDC ") + to_string(tdc),
            [this, &data, tdc](int id) {

                string plotTitle = string("TDC ") 
                    + to_string(tdc)
                    + string(" TDC Channels");

                if(!hasTab(plotTitle.data())) {

                    HistogramPlotter *tdcChannelPlot = new HistogramPlotter(
                        this,
                        data.plots.p_tdc_time_corrected[tdc],
                        plotTitle,
                        width,
                        height
                    );

                    buildTab(plotTitle, tdcChannelPlot);

                }

                // Switch to the new tab
                SetTab(plotTitle.data());

            }
        );

        ++tdcChannelPlotCount;

    }

    while(data.plots.p_tdc_time_corrected.size() < tdcChannelPlotCount) {

        // Remove extra entries and close associated tabs

        int tdc = tdcChannelPlotCount - 1;

        tdcMenu->removeEntry(string("TDC ") + to_string(tdc));

        string plotTitle = string("TDC ") 
            + to_string(tdc)
            + string(" TDC Channels");

        // We need to get the tab index

        int tabIndex = -1;
        for(int i = 0; i < GetNumberOfTabs(); ++i) {

            if(strcmp(GetTabTab(i)->GetString(), plotTitle.data()) == 0) {

                tabIndex = i;
                break;

            }

        }
        
        if(tabIndex != -1) {
                
            TGCompositeFrame *tab = GetTabContainer(tabIndex);

            RemoveTab(tabIndex, kFALSE);

            delete tab;

            relayoutNeeded = true;

        }

        --tdcChannelPlotCount;

    }

    data.unlock();

    if(relayoutNeeded) Layout();


}

bool TabPanel::isAttached() {

    return tabMenu != nullptr;

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

    if(id != 0) SetTab(id - 1);

}

bool TabPanel::hasTab(const string &label) {

    return GetTabTab(label.data()) != nullptr;

}