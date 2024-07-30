/**
 * @file StartDAQ.cpp
 *
 * @brief Entry point for the Phase 2 MiniDAQ online monitor.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "Components/DAQManager.h"

/**
 * Creates the graphical user interface component for the online monitor.
 */
DAQManager *buildGUI();

/**
 * Starts the UI event loop.
 * 
 * @param refreshRate The loop rate for the UI event loop, in Hz.
 */
void startUILoop(double refreshRate);

/**
 * Stops the UI event loop.
 */
void stopUILoop();