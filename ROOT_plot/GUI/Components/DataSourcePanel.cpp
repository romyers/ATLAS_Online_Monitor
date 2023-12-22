#include "DataSourcePanel.h"

#include <string>

using namespace std;
using namespace Muon;

ClassImp(DataSourcePanel);

DataSourcePanel::DataSourcePanel(const TGWindow *p) 
    : TGVerticalFrame(p) {

    ///////////////////////////////////////////////////////////////////////////

    sourceTypePanel = new TGVerticalFrame(this);
    AddFrame(sourceTypePanel, new TGLayoutHints(kLHintsLeft, 10, 10, 10, 10));

        dataSourceLabel = new TGLabel(sourceTypePanel, "Data Source:");
        sourceTypePanel->AddFrame(dataSourceLabel, new TGLayoutHints(kLHintsLeft));

        menu = new TGButtonGroup(sourceTypePanel, "", kVerticalFrame);
        sourceTypePanel->AddFrame(menu, new TGLayoutHints(kLHintsCenterX));

            fileButton   = new TGRadioButton(menu, "DAT File");
            menu->AddFrame(fileButton, new TGLayoutHints(kLHintsLeft));

            fileSelector = new FileSelector(menu);
            menu->AddFrame(fileSelector, new TGLayoutHints(kLHintsLeft, 30, 0, 10, 0));

            deviceButton = new TGRadioButton(menu, "Network Device");
            menu->AddFrame(deviceButton, new TGLayoutHints(kLHintsLeft));

            deviceSelector = new DeviceSelector(menu);
            menu->AddFrame(deviceSelector, new TGLayoutHints(kLHintsLeft, 30, 0, 10, 0));

    ///////////////////////////////////////////////////////////////////////////

    makeConnections();

    fileButton  ->Resize(50, 20);
    deviceButton->Resize(50, 20);

}

void DataSourcePanel::makeConnections() {

    fileButton  ->Connect("Clicked()", "DataSourcePanel", this, "setFileSource()"  );
    deviceButton->Connect("Clicked()", "DataSourcePanel", this, "setDeviceSource()");

    deviceSelector->Connect(
        "Selected(const char*)", 
        "DataSourcePanel", 
        this, 
        "selectedDevice(const char*)"
    );

    fileSelector->Connect(
        "TextChanged(const char*)",
        "DataSourcePanel",
        this,
        "selectedFile(const char*)"
    );

}

DataSourcePanel::~DataSourcePanel() {

}

void DataSourcePanel::setDeviceSelectorEntry(const string &entry) {

    deviceSelector->setDeviceName(entry.data());

}

void DataSourcePanel::setFileSelectorEntry(const string &entry) {

    fileSelector->setFilename(entry);

}

void DataSourcePanel::setDeviceSelectorOptions(const vector<string> &entries) {

    deviceSelector->setOptions(entries);

}

void DataSourcePanel::selectedFileSource  () { Emit("selectedFileSource()"  ); }
void DataSourcePanel::selectedDeviceSource() { Emit("selectedDeviceSource()"); }

void DataSourcePanel::selectedDevice(const char* selection) {

    Emit("selectedDevice(const char*)", selection);

}

void DataSourcePanel::selectedFile(const char *selection) {

    Emit("selectedFile(const char*)", selection);

}
    
void DataSourcePanel::setFileSource() {

    if(fileButton->GetState() != kButtonDown) {

        fileButton->SetState(kButtonDown);

    }

    selectedFileSource();

    fileSelector->enable();
    deviceSelector->disable();

}

void DataSourcePanel::setDeviceSource() {

    if(deviceButton->GetState() != kButtonDown) {

        deviceButton->SetState(kButtonDown);

    }

    selectedDeviceSource();

    fileSelector->disable();
    deviceSelector->enable();

}

void DataSourcePanel::enable() {

    dataSourceLabel->Enable();

    fileButton  ->SetEnabled(true);
    deviceButton->SetEnabled(true);

    if(fileButton->GetState() & kButtonDown) {

        fileSelector->enable();

    }

    if(deviceButton->GetState() & kButtonDown) {

        deviceSelector->enable();

    }

}

void DataSourcePanel::disable() {

    dataSourceLabel->Disable();

    fileButton  ->SetEnabled(false);
    deviceButton->SetEnabled(false);

    fileSelector  ->disable();
    deviceSelector->disable();

}