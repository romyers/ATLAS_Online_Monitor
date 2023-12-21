/**
 * @file RunView.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "DAQMonitor/Views/Components/RunStats.h"

class RunView : public TGGroupFrame {

public:

	RunView(const TGWindow *p);

	void update();

private:

	// VIEW

	RunStats *runStats      ;

	// CONNECTIONS

	void makeConnections();

};