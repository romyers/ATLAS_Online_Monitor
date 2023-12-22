#include "SigHandlers.h"

#include <signal.h>

using namespace std;

void setTerminationHandlers(void (*handler)(int)) {

    struct sigaction signalHandler;
    sigemptyset(&signalHandler.sa_mask);
    signalHandler.sa_flags = 0;
    signalHandler.sa_handler = handler;

    sigaction(SIGTERM, &signalHandler, nullptr);
    sigaction(SIGINT , &signalHandler, nullptr);
    sigaction(SIGQUIT, &signalHandler, nullptr);

}