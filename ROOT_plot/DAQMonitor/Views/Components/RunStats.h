/**
 * @file RunStats.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGHtml.h"

#include "DAQMonitor/Views/Components/PairTable.h"

class RunStats : public TGVerticalFrame {

public:

	RunStats(
		const TGWindow *parent
	);

	void update();

	void teardown();

private:

	// DATA

	PairTable statsTable;

	// VIEW

	TGHtml *table;

	// CONNECTIONS

	void makeConnections ();
	void breakConnections();

};