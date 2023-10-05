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

using namespace std;

class Terminator {

public:

    Terminator     (      Terminator &other) = delete;
    void operator= (const Terminator &other) = delete;

    bool isTerminated() const;

    void terminate();

    static Terminator &getInstance();

private:

    Terminator();

    bool terminateFlag;

    mutable mutex termLock; // We're C++11-compliant -- no shared_mutex

};

Terminator::Terminator() : terminateFlag(false) {}

bool Terminator::isTerminated() const {

    bool result;

    termLock.lock();
    result = terminateFlag;
    termLock.unlock();

    return result;

}

void Terminator::terminate() {

    termLock.lock();
    terminateFlag = true;
    termLock.unlock();

}

Terminator &Terminator::getInstance() {

    static Terminator instance;

    return instance;

}