/**
 * @file SiglHandlers.cpp
 *
 * @brief Handlers for system signals.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include "src/ProgramControl/Terminator.cpp"

using namespace std;

void forceExit             (int    signal       );
void flagForTermination    (int    signal       );
void setTerminationHandlers(void (*handler)(int));

void forceExit(int signal) {

    cout << endl << "Forced shutdown." << endl;

    exit(0);

}

void flagForTermination(int signal) {

    cout << endl << "Beginning shutdown." << endl;

    Terminator::getInstance().terminate();

    // This makes it so user can force shutdown with ctrl+c twice
    setTerminationHandlers(forceExit);

}

void setTerminationHandlers(void (*handler)(int)) {

    struct sigaction signalHandler;
    sigemptyset(&signalHandler.sa_mask);
    signalHandler.sa_flags = 0;
    signalHandler.sa_handler = handler;

    sigaction(SIGTERM, &signalHandler, NULL);
    sigaction(SIGINT , &signalHandler, NULL);
    sigaction(SIGQUIT, &signalHandler, NULL);

}