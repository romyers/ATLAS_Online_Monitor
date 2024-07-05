/**
 * @file DeviceSelectorPanel.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <string>

#include "TGComboBox.h"
#include "TGLabel.h"

class DeviceSelector : public TGVerticalFrame {

ClassDefOverride(DeviceSelector, 0);

public:

    DeviceSelector(const TGWindow *p);

    virtual ~DeviceSelector() override;

    // SIGNALS

    void Selected(const char *entry); // *SIGNAL*
    void Selected(Int_t id);          // *SIGNAL*

    // SLOTS

    void handleSelected(Int_t id);

    // METHODS

    void initialize();

    void setOptions(const std::vector<std::string> &entries);

    bool ready();

    void enable();
    void disable();
    
    void setDeviceName(const char *name);

private:

    // DATA

    bool initialized;

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGLabel    *label   ;
    TGComboBox *dropdown;
    ///////////////////////////////////////////////////////////////////////////

    // CONNECTIONS

    void makeConnections();

};