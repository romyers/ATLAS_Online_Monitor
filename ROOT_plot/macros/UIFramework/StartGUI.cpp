/**
 * @file StartGUI.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <thread>

#include "monitorConfig.cpp"

#include "macros/GUIManager.cpp"

using namespace std;

void StartGUI() {

    GUIManager &GUI = *GUIManager::getInstance();

    Canvas *canvas = GUI.makeCanvas("temp");

    Histogram hist;

    canvas->addElement(&hist);

    while(true) {

        GUI.refresh();

        this_thread::sleep_for(chrono::milliseconds((int)(1000 / GUI_REFRESH_RATE)));

    }

}