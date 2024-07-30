/**
 * @file ConfFileSelector.h
 *
 * @brief Provides an interface for selecting the conf file.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

class ConfFileSelector : public TGVerticalFrame {

ClassDef(ConfFileSelector, 0);

public:

    ConfFileSelector(const TGWindow *p);
    virtual ~ConfFileSelector() override;

    // SIGNALS

    void entryChanged(const char *text); // *SIGNAL*

    // METHODS

    void enable ();
    void disable();

    void setEntry(const std::string &entry);

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGVerticalFrame *selectorPanel;

        TGLabel     *label;
        TGTextEntry *entryField;
            
    ///////////////////////////////////////////////////////////////////////////

    // CONNECTIONS

    void makeConnections();

};