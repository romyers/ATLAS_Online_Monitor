/**
 * @file MonitorHooks.cpp
 * 
 * Defines lifecycle hooks for the monitor. Together with 
 * src/DataModel/DAQData.cpp, this provides an interface for new modules to
 * use.
 * 
 * Each hook may be treated as a void 'main function' (without command line 
 * arguments) that runs at the right time in the program. Depending on which
 * lifecycle hook you're programming in, the DAQData variable may be populated
 * with processed events and plots. See src/DataModel/DAQData.cpp for more.
 * 
 * DAQData is currently passed in by const reference, but feel free to 
 * remove the const condition if you need to, keeping in mind that DAQData
 * is not meant to be written to.
 * 
 * src/ProgramControl/Terminator.cpp provides a universal termination condition
 * that can be checked for to determine if the program has been flagged for
 * termination (as it will, for example, on CTRL+C). This is the intended way
 * for you to terminate anything you want to be running concurrently with
 * the decoder. For example, you might want to have a thread with a loop that 
 * looks like:
 * 
 * |  while(true) {
 * |
 * |      // do something ad nauseam
 * |
 * |  }
 * 
 * This loop is better written:
 * 
 * |  while(!Terminator::getInstance().isTerminated()) {
 * |
 * |	     // do something until the program finishes
 * |
 * |  }
 * 
 * CAUTION: 
 * 
 *   -- DAQData.cpp provides methods for locking and unlocking an internal
 *      mutex, but it is not inherently threadsafe. It is used by multiple 
 *      threads in the monitor program, so it's important to always lock it 
 *      before using it and unlock it after. Try not to lock it longer than 
 *      you need to in order to avoid blocking other threads.
 * 
 *   -- DAQData::processedEvents may be empty at any point -- even in the 
 *      finishedRun and updatedData hooks. Check it before you use it.
 * 
 *   -- To prevent interference with other parts of the program, it's best
 *      to treat DAQData as a read-only object.
 * 
 *   -- beforeUpdateData and updatedData are called by the decoding thread,
 *      while beforeStartRun, startedRun, and finishedRun are called by the
 *      main thread. beforeStartRun and finishedRun shouldn't run concurrently
 *      with the other threads, but startedRun will. Make sure to lock guard
 *      any data shared between these hooks.
 * 
 *   -- Each hook must terminate for the program to properly shut down. The
 *      main program blocks SIGINT, so you might find CTRL+C not to behave
 *      as expected when it hangs. You can force shutdown by pressing
 *      CTRL+C twice should this occur.
 *        >> You'll find that the program takes a few seconds to shut down
 *           normally, even on SIGINT. This is (for now) normal behavior.
 *           You can press CTRL+C twice to force shutdown. Hopefully,
 *           this delay will eventually be eliminated.
 * 
 *   -- Each hook is called directly during program flow. Separate threads
 *      are not created for them, so if you need something to run concurrently
 *      with the rest of the program without blocking it, you'll need to create
 *      a thread yourself. It is safe to join threads in startedRun or 
 *      finishedRun, however, as both of these are called after the monitor has
 *      been initialized and its threads started, so the monitor won't block. 
 *      The best place to create threads, consequently, is in these two 
 *      functions.
 * 
 *   -- Join any thread you create to ensure that it shuts down nicely before
 *      the program is terminated. Again, it is safe to join threads in
 *      startedRun and finishedRun.
 * 
 *   -- Each of startRun, beforeStartRun, and finishedRun are only called
 *      once during execution of the program. beforeUpdataData and updatedData,
 *      however, are called many, many times. Avoid e.g. starting threads
 *      or initializing data structures you only want one of in 
 *      beforeUpdataData or updatedData.
 * 
 * Feel free to edit this file and this note as desired, as long as you 
 * preserve the lifecycle hooks and their names. Also, there is an
 * error logger used by other parts of the program, but it's not robust
 * yet. Feel free to use cout and cerr. These streams are not guaranteed
 * to be threadsafe, but they work atomically enough on this system, and 
 * there will eventually be a threadsafe logging system to replace them.
 * 
 * I've not assumed a strong understanding of multithreading pitfalls or best
 * practices in writing these notes. Feel free to skim any multithreading 
 * cautions if you're proficient with it.
 * 
 */

#include "MonitorHooks.h"

using namespace std;

/**
* This is called at monitor startup, before any threads are started and
* before data capture and decoding begins.
*/
void MonitorHooks::beforeStartRun(const DAQData &data) {

}

/**
* This is called after the data capture and decoding threads are started.
*/
void MonitorHooks::startedRun(const DAQData &data) {

}

/**
* This is called at the very end of a run, after all data capture and
* decoding is finished and their respective threads joined. At this
* point, DAQData will be in its final state.
*/
void MonitorHooks::finishedRun(const DAQData &data) {

}

/**
* This is called at the beginning of each decode loop, before captured
* data is decoded and aggregated.
*/
void MonitorHooks::beforeUpdateData(const DAQData &data) {


}

/**
* This is called at the end of each decode loop, after captured data
* is decoded and aggregated.
*/
void MonitorHooks::updatedData(const DAQData &data) {

}