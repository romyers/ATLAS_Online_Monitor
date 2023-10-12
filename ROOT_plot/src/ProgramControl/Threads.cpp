/**
 * @file Threads.cpp
 *
 * @brief Stores program threads in a common place.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <thread>

using namespace std;

namespace Muon {
namespace ProgramFlow {

    vector<thread> threads;

    mutex threadLock;

    void joinAllThreads() {

        for(thread &t : threads) {

            t.join();

        }

    }

}
}