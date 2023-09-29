#pragma once

/**
 * The approximate rate at which monitor data is refreshed. Note that the
 * monitor will fall short of this rate if it must process too much data
 * at a time.
 */
const double DATA_REFRESH_RATE = 10.; // Hz

/**
 * The approximate rate at which the GUI is refreshed.
 */
const double GUI_REFRESH_RATE  = 30.; //Hz

/**
 * A function setting the run number. Can be edited to control how runs are
 * distinguished.
 */
inline int getRunNumber() { return 0; }