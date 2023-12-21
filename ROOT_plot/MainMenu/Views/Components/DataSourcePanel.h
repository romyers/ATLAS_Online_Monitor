/**
 * @file DataSourcePanel.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButtonGroup.h"
#include "TGButton.h"

#include "MainMenu/Views/Components/DeviceSelectorPanel.h"
#include "MainMenu/Views/Components/FileSelectorPanel.h"

class DataSourcePanel : public TGVerticalFrame {

public:

    DataSourcePanel(const TGWindow *p);
    virtual ~DataSourcePanel() override;

    // OPERATIONS

    void showFileSelector       ();
    void showDeviceSelector     ();

    void revertSettings         ();
    void commitSettings         ();

    void enable                 ();
    void disable                ();

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

        TGTextButton *applyButton;
        TGTextButton *revertButton;
    ///////////////////////////////////////////////////////////////////////////

    // TODO: Add a flag determining whether this has unsaved changes

    // CONNECTIONS

    void makeConnections();

};