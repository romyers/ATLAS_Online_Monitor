
#pragma once

#include <string>

#include "src/EthernetCapture/DeviceManager.cpp"

using namespace std;

class DeviceSelector : public TGHorizontalFrame {

    RQ_OBJECT("DeviceSelector");

public:

    DeviceSelector(const TGWindow *p);

    virtual ~DeviceSelector() override;

    // METHODS

    void initialize();

    bool ready();

    void enable();
    void disable();

    void setDeviceName(char *name);

    // SIGNALS

    void Selected(char *name) {}

    // OPERATIONS 

    void emitDeviceName(char *name); // *SIGNAL*

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGLabel    *label   ;
    TGComboBox *dropdown;
    ///////////////////////////////////////////////////////////////////////////

    bool initialized;

};

void DeviceSelector::setDeviceName(char *name) {

    // FIXME: The current entry must be cleared if name is blank
    if(name == "") {

        dropdown->Select(-1);
        emitDeviceName(name);
        return;

    }

    int entryID = dropdown->FindEntry(name)->EntryId();

    if(!entryID) {

        throw NetworkDeviceException("Selected device does not exist.");

    }

    dropdown->Select(entryID, false);

    emitDeviceName(name);

}

void DeviceSelector::emitDeviceName(char *name) {

    Emit("Selected(char*)", name);

}

DeviceSelector::DeviceSelector(const TGWindow *p) 
    : TGHorizontalFrame(p), initialized(false) {

    label = new TGLabel(this, "Select Device:");
    AddFrame(label, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));


    dropdown = new TGComboBox(this);
    AddFrame(dropdown, new TGLayoutHints(kLHintsCenterY, 5, 5, 5, 5));

    dropdown->Connect(
        "Selected(char*)", 
        "DeviceSelector", 
        this, 
        "emitDeviceName(char*)"
    );

    dropdown->Resize(150, 20);

}

DeviceSelector::~DeviceSelector() {

    TGHorizontalFrame::~TGHorizontalFrame();

}

void DeviceSelector::initialize() {

    DeviceManager devices;
    devices.initialize();

    vector<PCapDevice> deviceList = devices.getAllDevices();

    for(int i = 0; i < deviceList.size(); ++i) {

        dropdown->AddEntry(deviceList[i].name(), i + 1);

    }

    initialized = true;

}

bool DeviceSelector::ready() {

    return initialized;

}


void DeviceSelector::enable() {

    dropdown->SetEnabled(true);

    label->Enable();

}

void DeviceSelector::disable() {

    dropdown->SetEnabled(false);

    label->Disable();

}