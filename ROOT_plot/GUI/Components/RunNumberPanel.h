/**
 * @file RunNumberPanel.h
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

class RunNumberPanel : public TGVerticalFrame {

ClassDef(RunNumberPanel, 0);

public:

    RunNumberPanel(const TGWindow *p);
    virtual ~RunNumberPanel() override;

    // SIGNALS

    void TextChanged(const char *text); // *SIGNAL*

    // METHODS

    void enable();
    void disable();

    int getRunNumber();

    void setRunNumber(int runNumber);

	void update();

private:

    // VIEW

    ///////////////////////////////////////////////////////////////////////////
	TGVerticalFrame *entryPanel;

		TGLabel      *label        ;
		TGTextEntry  *entryField   ;
    ///////////////////////////////////////////////////////////////////////////

    // CONNECTIONS

    void makeConnections();

};