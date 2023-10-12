/**
 * @file SigHandlers.cpp
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

    gApplication->Terminate(0);

    exit(0); // In case Terminate on gApplication doesn't do the job.

}

void flagForTermination(int signal) {

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