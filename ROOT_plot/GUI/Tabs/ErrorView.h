/**
 * @file ErrorView.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGHtml.h"

#include "UITab.h"

#include "GUI/Components/PairTable.h"

class ErrorView : public UITab {

public:

	ErrorView(const TGWindow *p);

	virtual ~ErrorView();

	void update();

private:

	// DATA

	PairTable stringTable;

	// VIEW

	TGHtml *errorPanel;

};