/**
 * @file DataSourcePanel.h
 *
 * @brief Provides an interface for selecting the DAQ data source.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <vector>

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButtonGroup.h"
#include "TGButton.h"

#include "DeviceSelectorPanel.h"
#include "FileSelectorPanel.h"

class DataSourcePanel : public TGVerticalFrame {

ClassDefOverride(DataSourcePanel, 0);

public:

    DataSourcePanel(const TGWindow *p);
    virtual ~DataSourcePanel() override;

    // SIGNALS

    void selectedFileSource  (); // *SIGNAL*
    void selectedDeviceSource(); // *SIGNAL*

    void selectedDevice(const char *selection); // *SIGNAL*
    void selectedFile  (const char *selection); // *SIGNAL*

    // OPERATIONS

    void setFileSource  ();
    void setDeviceSource();

    void enable         ();
    void disable        ();

    void setDeviceSelectorOptions(
        const std::vector<std::string> &entries
    );
    void setDeviceSelectorEntry(const std::string &entry);
    void setFileSelectorEntry(const std::string &entry);

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
            
    ///////////////////////////////////////////////////////////////////////////

    // TODO: Add a flag determining whether this has unsaved changes

    // CONNECTIONS

    void makeConnections();

};