/**
 * @file DAQApp.h
 * 
 * @brief Main entry point for the DAQMonitor GUI. Defines a high-level
 * interface for controlling the GUI application, including ROOT setup and
 * the UI loop.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "TApplication.h"

#include <string>

namespace DAQ {

    class App {

    ClassDef(App, 0);

    public:

        /**
         * @brief Construct a new App object.
         */
        App(const std::string &title);
        ~App();

        /**
         * @brief Runs the main UI loop, continuously processing events.
         * 
         * This is a looping function that will run the main event loop at the
         * specified refresh rate until stop() is called. The function will not
         * return until the GUI is stopped, so don't expect any code after a
         * call to this function to run while the GUI is still running.
         * 
         * @param refreshRate The refresh rate of the GUI in Hertz.
         */
        void run(double refreshRate);

        /**
         * @brief Stops the main UI loop.
         * 
         * This is called automatically when the window is closed by the user.
         * Anything that closes the window should make sure this function is
         * called.
         * 
         * @note The application may complete up to one full UI loop iteration
         * before stoppping, so don't expect the GUI to stop immediately. You
         * can be sure that the GUI has fully stopped once the call to run()
         * has returned.
         */
        void stop();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

    private:

        TApplication app;

        /* 
         * Flag controlling the main UI loop. The loop will run until it finds
         * this flag set to false.
         */
        bool runLoop = true;

    };

}