/**
 * @file ProgressBar.h
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TGWindow.h"
#include "TGProgressBar.h"

class ProgressBar : public TGTransientFrame {

public:

	ProgressBar(const TGWindow *p);

	void increment(float inc);

private:

	// VIEW

	TGHProgressBar *bar;

};