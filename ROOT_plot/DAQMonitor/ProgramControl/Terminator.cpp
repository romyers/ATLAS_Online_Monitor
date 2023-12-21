#include "Terminator.h"

using namespace std;

Terminator::Terminator() {}

bool Terminator::isTerminated(const string &flag) const {

    // TODO: Check performance

    bool result;

    termLock.lock();

    if(terminateFlags.count(flag) == 0) {

        result = false;

    } else {

        result = true;

    }

    // We always terminate if the terminate all flag is set
    if(terminateFlags.count(TERMINATE_ALL_FLAG) != 0) {

        result = true;

    }

    termLock.unlock();

    return result;

}

void Terminator::terminate(const string &flag) {

    termLock.lock();
    terminateFlags.insert(flag);
    termLock.unlock();

}

void Terminator::clearFlag(
    const string &flag
) {

    terminateFlags.erase(flag);

}

Terminator &Terminator::getInstance() {

    static Terminator instance;

    return instance;

}