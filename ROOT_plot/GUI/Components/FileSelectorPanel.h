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

public:

    FileSelector(const TGWindow *p);
    virtual ~FileSelector() override;

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

};