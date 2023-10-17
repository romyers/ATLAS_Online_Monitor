/**
 * @file Terminator.cpp
 *
 * @brief Threadsafe singleton class. Conditioning thread loops on 
 * Terminator::isTerminated() allows detached threads to be terminated cleanly 
 * with a call to Terminator::Terminate().
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */
#pragma once

#include <mutex>
#include <set>

using namespace std;

const string TERMINATE_ALL_FLAG = "";

class Terminator {

public:

    Terminator     (      Terminator &other) = delete;
    void operator= (const Terminator &other) = delete;

    bool isTerminated(const string &flag = TERMINATE_ALL_FLAG) const;

    void terminate(const string &flag = TERMINATE_ALL_FLAG);
    void clearFlag(const string &flag);

    static Terminator &getInstance();

private:

    Terminator();

    set<string> terminateFlags;

    mutable mutex termLock; // We're C++11-compliant -- no shared_mutex

};

Terminator::Terminator() {}

bool Terminator::isTerminated(
    const string &flag = TERMINATE_ALL_FLAG
) const {

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

void Terminator::terminate(
    const string &flag = TERMINATE_ALL_FLAG
) {

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