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
const double GUI_REFRESH_RATE  = 60.; //Hz

/**
 * Update UI elements every [UI_UPDATE_FRAMES] frames, i.e.
 * GUI_REFRESH_RATE / UI_UPDATE_FRAMES times per second.
 */
const int    UI_UPDATE_FRAMES  = 15  ; // Frames

/**
 * A function setting the run number. Can be edited to control how runs are
 * distinguished.
 */
inline int getRunNumber() {

    return 0;

}