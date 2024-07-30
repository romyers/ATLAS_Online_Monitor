/**
 * @file Threads.h
 *
 * @brief Stores program threads in a common place.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <thread>
#include <mutex>

namespace ProgramFlow {

    extern std::vector<std::thread> threads;

    extern std::mutex threadLock;

    /**
     * Blocks the calling thread until each thread in ProgramFlow::threads
     * has been joined.
     */
    void joinAllThreads();

}