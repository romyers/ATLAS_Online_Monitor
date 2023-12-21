/**
 * @file RunView.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGHtml.h"

#include "PairTable.h"

class RunView : public TGGroupFrame {

public:

	RunView(const TGWindow *p);

	void update();

	void teardown();

private:

	// DATA

	PairTable statsTable;

	// VIEW

	TGHtml *table;

};