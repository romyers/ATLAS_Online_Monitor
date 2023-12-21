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

#include <string>
#include <set>
#include <mutex>

const std::string TERMINATE_ALL_FLAG = "";

class Terminator {

public:

    Terminator     (      Terminator &other) = delete;
    void operator= (const Terminator &other) = delete;

    bool isTerminated(const std::string &flag = TERMINATE_ALL_FLAG) const;

    void terminate(const std::string &flag = TERMINATE_ALL_FLAG);
    void clearFlag(const std::string &flag);

    static Terminator &getInstance();

private:

    Terminator();

    std::set<std::string> terminateFlags;

    mutable std::mutex termLock; // We're C++11-compliant -- no shared_mutex

};