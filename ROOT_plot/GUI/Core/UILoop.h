/**
 * @file UILoop.h
 *
 * @brief Provides a UI event loop for ROOT GUI programs.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

/**
 * Starts the UI loop.
 */
void startLoop(double refreshRate, double dataRefreshRate);

/**
 * Stops the UI loop.
 */
void stopLoop();