/**
 * @file DeviceSelectorPanel.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TGComboBox.h"
#include "TGLabel.h"

class DeviceSelector : public TGVerticalFrame {

public:

    DeviceSelector(const TGWindow *p);

    virtual ~DeviceSelector() override;

    // METHODS

    void initialize();

    bool ready();

    void enable();
    void disable();

    std::string getDeviceName();
    void setDeviceName(char *name);

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGLabel    *label   ;
    TGComboBox *dropdown;
    ///////////////////////////////////////////////////////////////////////////

    bool initialized;

};