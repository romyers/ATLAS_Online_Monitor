/**
 * @file DataSourcePanel.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "macros/DAQState.cpp"

#include "macros/UIWindows/EntryWindow/Components/DeviceSelectorPanel.cpp"
#include "macros/UIWindows/EntryWindow/Components/FileSelectorPanel.cpp"

using namespace std;
using namespace Muon;

// TODO: I haven't been very rigorous with this. Get more rigorous.

class DataSourcePanel : public TGVerticalFrame {

public:

    DataSourcePanel(const TGWindow *p);
    virtual ~DataSourcePanel() override;

    // OPERATIONS

    void showFileSelector       ();
    void showDeviceSelector     ();

    void revertSettings         ();
    void commitSettings         ();

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGVerticalFrame *sourceTypePanel;

        TGLabel *dataSourceLabel;

        TGButtonGroup *menu;

            TGRadioButton  *fileButton    ;
            FileSelector   *fileSelector  ;

            TGRadioButton  *deviceButton  ;
            DeviceSelector *deviceSelector;


    TGButtonGroup *commitPanel;

        TGTextButton *revertButton;
        TGTextButton *applyButton;
    ///////////////////////////////////////////////////////////////////////////

    // TODO: Add a flag determining whether this has unsaved changes

    // CONNECTIONS

    void makeConnections();

};

DataSourcePanel::DataSourcePanel(const TGWindow *p) 
    : TGVerticalFrame(p) {

    // TODO: These long GUI layout definitions are hard to read. Break them up.
    //       Stretch goal -- make an xml parser for this

    ///////////////////////////////////////////////////////////////////////////

    sourceTypePanel = new TGVerticalFrame(this);
    AddFrame(sourceTypePanel, new TGLayoutHints(kLHintsLeft, 10, 10, 10, 10));

        dataSourceLabel = new TGLabel(sourceTypePanel, "Data Source:");
        sourceTypePanel->AddFrame(dataSourceLabel, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));

        menu = new TGButtonGroup(sourceTypePanel, "", kVerticalFrame);
        sourceTypePanel->AddFrame(menu, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

            fileButton   = new TGRadioButton(menu, "DAT File");
            menu->AddFrame(fileButton, new TGLayoutHints(kLHintsLeft));

            fileSelector = new FileSelector(menu);
            menu->AddFrame(fileSelector, new TGLayoutHints(kLHintsLeft, 30, 0, 10, 0));

            deviceButton = new TGRadioButton(menu, "Network Device");
            menu->AddFrame(deviceButton, new TGLayoutHints(kLHintsLeft));

            deviceSelector = new DeviceSelector(menu);
            menu->AddFrame(deviceSelector, new TGLayoutHints(kLHintsLeft, 30, 0, 10, 0));

    ///////////////////////////////////////////////////////////////////////////


    commitPanel = new TGButtonGroup(this, "", kHorizontalFrame);
    AddFrame(commitPanel, new TGLayoutHints(kLHintsRight, 0, 0, 0, 0));

        revertButton = new TGTextButton(commitPanel, "Undo Changes");
        applyButton  = new TGTextButton(commitPanel, "Apply Changes" );

    ///////////////////////////////////////////////////////////////////////////

    makeConnections();

    fileButton  ->Resize(50, 20);
    deviceButton->Resize(50, 20);

    revertButton->Resize(50, 20);
    applyButton ->Resize(50, 20);

    // DEFAULTS
    // FIXME: fileSelector is not initially hidden.
    deviceButton->SetState(kButtonDown);
    deviceSelector->initialize();
    showDeviceSelector();

    revertSettings();

}

void DataSourcePanel::makeConnections() {

    fileButton  ->Connect("Clicked()", "DataSourcePanel", this, "showFileSelector()"        );
    deviceButton->Connect("Clicked()", "DataSourcePanel", this, "showDeviceSelector()"      );

    revertButton->Connect("Clicked()", "DataSourcePanel", this, "revertSettings()"          );
    applyButton ->Connect("Clicked()", "DataSourcePanel", this, "commitSettings()"          );

}

DataSourcePanel::~DataSourcePanel() {

    TGVerticalFrame::~TGVerticalFrame();

}
    
void DataSourcePanel::showFileSelector() {

    fileSelector->enable();
    deviceSelector->disable();

}

void DataSourcePanel::showDeviceSelector() {

    fileSelector->disable();
    deviceSelector->enable();

}

void DataSourcePanel::revertSettings() {


    State::DAQState state = State::DAQState::getState();

    char buffer[256];
    strcpy(buffer, state.inputDevicename.data());
    deviceSelector->setDeviceName(buffer);

    fileSelector->setFilename(state.inputFilename);

    if(state.dataSource == State::DAT_FILE_SOURCE) {

        fileButton->Clicked();
        fileButton->SetState(kButtonDown);

    } else if(state.dataSource == State::NETWORK_DEVICE_SOURCE) {

        deviceButton->Clicked();
        deviceButton->SetState(kButtonDown);

    } else {

        throw std::logic_error("DataSourcePanel::revertSettings cannot find a valid data source in DAQState.");

    }

}

void DataSourcePanel::commitSettings() {

    // NOTE: We can ensure the DAQState won't go out of date if we lock
    //       it. But there shouldn't be anything messing with DAQState 
    //       concurrently with commitSettings, so let's just guarantee
    //       with validation and come back later if we need to.

    State::DAQState state = State::DAQState::getState();

    // TODO: Validation -- e.g. network device must exist

    state.inputFilename = fileSelector->getFilename();
    state.inputDevicename = deviceSelector->getDeviceName();

    if(fileButton->GetState() == kButtonDown) {

        state.dataSource = State::DAT_FILE_SOURCE;

    } else if(deviceButton->GetState() == kButtonDown) {

        state.dataSource = State::NETWORK_DEVICE_SOURCE;

    } else {

        throw std::logic_error("DataSourcePanel::commitSettings cannot find a data input source type selection.");

    }

    bool success = state.commit();
    
    if(!success) {

        throw std::logic_error(
            "DataSourcePanel::commitSettings could not commit settings. "
            "Make sure nothing is committing DAQState concurrently with "
            "commitSettings, or reimplement commitSettings to handle "
            "concurrent state commits."
        );

    }

}