#include "DeviceSelectorPanel.h"

#include <string>

using namespace std;

ClassImp(DeviceSelector);

DeviceSelector::DeviceSelector(const TGWindow *p) 
    : TGVerticalFrame(p), initialized(false) {

    label = new TGLabel(this, "Select Device:");
    AddFrame(label, new TGLayoutHints(kLHintsLeft));


    dropdown = new TGComboBox(this);
    AddFrame(dropdown, new TGLayoutHints(kLHintsLeft));

    dropdown->Resize(150, 20);

    makeConnections();

}

void DeviceSelector::makeConnections() {

    dropdown->Connect("Selected(Int_t)", "DeviceSelector", this, "handleSelected(Int_t)");

}

void DeviceSelector::setDeviceName(const char *name) {

    TGLBEntry *entry = dropdown->FindEntry(name);

    if(!entry) {

        // We can handle this quietly. If user selects a nonexistent device,
        // we've done something very wrong. But the program might try it
        // on settings state load if a previously used device has been 
        // disconnected. In that case, we really just want to clear the default
        // setting.
        dropdown->Select(0);
        return;

    }

    dropdown->Select(entry->EntryId(), false);

}

DeviceSelector::~DeviceSelector() {

}

void DeviceSelector::Selected(const char *entry) {

    Emit("Selected(const char *)", entry);

}

void DeviceSelector::Selected(Int_t id) {

    Emit("Selected(Int_t)", id);

}

void DeviceSelector::handleSelected(Int_t id) {

    Selected(id);
    Selected(dropdown->GetSelectedEntry()->GetTitle());

}

void DeviceSelector::setOptions(const vector<string> &entries) {

    for(int i = 0; i < entries.size(); ++i) {

        dropdown->AddEntry(entries[i].data(), i);

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

    // TODO: Selections in the box are not grayed out

    dropdown->SetEnabled(false);

    label->Disable();

}