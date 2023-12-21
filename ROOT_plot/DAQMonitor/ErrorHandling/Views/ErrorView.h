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

#include "DAQMonitor/Views/Components/PairTable.h"

class ErrorView : public TGCompositeFrame {

public:

	ErrorView(const TGWindow *p);

	~ErrorView();

	void update();

private:

	// DATA

	PairTable stringTable;

	// VIEW

	TGHtml *errorPanel;

	// CONNECTIONS

	void makeConnections ();
	void breakConnections();

};