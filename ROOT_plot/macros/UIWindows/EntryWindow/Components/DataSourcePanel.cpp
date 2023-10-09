

#pragma once;

#include <string>

#include "macros/DAQState.cpp"

#include "macros/UIWindows/EntryWindow/Components/DeviceSelectorPanel.cpp"
#include "macros/UIWindows/EntryWindow/Components/FileSelectorPanel.cpp"

using namespace std;
using namespace Muon;

class DataSourcePanel : public TGVerticalFrame {

public:

    DataSourcePanel(const TGWindow *p);
    virtual ~DataSourcePanel() override;

    // OPERATIONS

    void showFileSelector       (          );
    void showDeviceSelector     (          );

    void setTypeToFile          (          );
    void setTypeToNetworkDevice (          );

    void setDeviceName          (char *name);
    void setFilename            (char *name);

    void revertSettings         (          );
    void commitSettings         (          );

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGHorizontalFrame *sourceTypePanel;

        TGLabel *dataSourceLabel;

        TGButtonGroup *menu;

            TGRadioButton *fileButton;
            TGRadioButton *deviceButton;


    TGVerticalFrame *selectorPanel;

        DeviceSelector *deviceSelector;
        FileSelector   *fileSelector;

    TGButtonGroup *commitPanel;

        TGTextButton *revertButton;
        TGTextButton *applyButton;
    ///////////////////////////////////////////////////////////////////////////

    // DATA

    int    dataSourceType ;

    string inputDevicename;
    string inputFilename  ;

    // TODO: Add a flag determining whether this has unsaved changes

    // CONNECTIONS

    void makeConnections();

};

void DataSourcePanel::makeConnections() {

    // TODO: Should we do the GUI connections and data model connections
    //       separately?

    fileButton    ->Connect("Clicked()", "DataSourcePanel", this, "showFileSelector()"        );
    fileButton    ->Connect("Clicked()", "DataSourcePanel", this, "setTypeToFile()"           );

    deviceButton  ->Connect("Clicked()", "DataSourcePanel", this, "showDeviceSelector()"      );
    deviceButton  ->Connect("Clicked()", "DataSourcePanel", this, "setTypeToNetworkDevice()"  );

    deviceSelector->Connect("Selected(char*)", "DataSourcePanel", this, "setDeviceName(char*)");
    fileSelector  ->Connect("Selected(char*)", "DataSourcePanel", this, "setFilename(char*)"  );

    revertButton  ->Connect("Clicked()", "DataSourcePanel", this, "revertSettings()"          );
    applyButton   ->Connect("Clicked()", "DataSourcePanel", this, "commitSettings()"          );

}

DataSourcePanel::DataSourcePanel(const TGWindow *p) 
    : TGVerticalFrame(p) {

    // TODO: These long GUI layout definitions are hard to read. Break them up.
    //       Stretch goal -- make an xml parser for this

    ///////////////////////////////////////////////////////////////////////////

    sourceTypePanel = new TGHorizontalFrame(this);
    AddFrame(sourceTypePanel, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

        dataSourceLabel = new TGLabel(sourceTypePanel, "Data Source:");
        sourceTypePanel->AddFrame(dataSourceLabel, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

        menu = new TGButtonGroup(sourceTypePanel, "", kHorizontalFrame);
        sourceTypePanel->AddFrame(menu, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

            fileButton   = new TGRadioButton(menu, "File"          );
            deviceButton = new TGRadioButton(menu, "Network Device");

    ///////////////////////////////////////////////////////////////////////////

    // TODO: Break this out into a custom element that holds a vector of panels and
    //       functionality for switching between them

    selectorPanel = new TGVerticalFrame(this);
    AddFrame(selectorPanel, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

        deviceSelector = new DeviceSelector(selectorPanel);
        selectorPanel->AddFrame(deviceSelector, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

        fileSelector = new FileSelector(selectorPanel);
        selectorPanel->AddFrame(fileSelector, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

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
    showDeviceSelector();
    dataSourceType = State::NETWORK_DEVICE_SOURCE;

    inputDevicename = "";
    inputFilename = "";

}

DataSourcePanel::~DataSourcePanel() {

    TGVerticalFrame::~TGVerticalFrame();

}
    
void DataSourcePanel::showFileSelector() {

    selectorPanel->HideFrame(deviceSelector);
    selectorPanel->ShowFrame(fileSelector);

}

void DataSourcePanel::showDeviceSelector() {


    if(!deviceSelector->ready()) {

        deviceSelector->initialize();

    }
    selectorPanel->HideFrame(fileSelector);
    selectorPanel->ShowFrame(deviceSelector);

}

void DataSourcePanel::setDeviceName(char *name) {

    inputDevicename = name;

}

void DataSourcePanel::setFilename(char *name) {

    inputFilename = name;

}

void DataSourcePanel::setTypeToFile() {

    dataSourceType = State::DAT_FILE_SOURCE;

}

void DataSourcePanel::setTypeToNetworkDevice() {

    dataSourceType = State::NETWORK_DEVICE_SOURCE;

}

void DataSourcePanel::revertSettings() {


    State::DAQState &state = State::DAQState::getState();

    char buffer[256];

    state.lock();

    // NOTE: Setting the device name will trigger an event that will update
    //       DataSourcePanel::inputDeviceName
    strcpy(buffer, state.inputDevicename.data());
    deviceSelector->setDeviceName(buffer);

    strcpy(buffer, state.inputFilename.data());
    fileSelector->setFilename(buffer);

    dataSourceType = state.dataSource;

    if(dataSourceType == State::NETWORK_DEVICE_SOURCE) {

        deviceButton->Clicked();
        deviceButton->SetState(kButtonDown);

    } else if(dataSourceType == State::DAT_FILE_SOURCE) {

        fileButton->Clicked();
        fileButton->SetState(kButtonDown);

    }

    state.unlock();

}

void DataSourcePanel::commitSettings() {

    State::DAQState &state = State::DAQState::getState();

    // TODO: Validation -- e.g. network device must exist

    state.lock();

    state.dataSource      = dataSourceType ;

    switch(dataSourceType) {

        case State::DAT_FILE_SOURCE:
            state.inputFilename = inputFilename;
            break;

        case State::NETWORK_DEVICE_SOURCE:
            state.inputDevicename = inputDevicename;
            break;

        default:
            throw logic_error("DataSourcePanel::commitSettings tried to commit bad data source type.");
            break;

    }

    state.unlock();

}