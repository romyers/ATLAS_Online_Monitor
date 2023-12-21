#include "SigHandlers.h"

#include <signal.h>

#include "TApplication.h"

#include "src/ProgramControl/Terminator.h"

using namespace std;

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