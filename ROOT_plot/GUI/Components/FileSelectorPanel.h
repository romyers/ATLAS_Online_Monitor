/**
 * @file FileSelectorPanel.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

class FileSelector : public TGVerticalFrame {

ClassDef(FileSelector, 0);

public:

    FileSelector(const TGWindow *p);
    virtual ~FileSelector() override;

    // SIGNALS

    void TextChanged(const char *text); // *SIGNAL*

    // METHODS

    void enable();
    void disable();

    std::string getFilename();

    void setFilename(const std::string &name);

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
    TGLabel      *label        ;
    TGTextEntry  *entryField   ;
    ///////////////////////////////////////////////////////////////////////////

    // CONNECTIONS

    void makeConnections();

};