#include "DAQMonitor/DataModel/DAQData.h"

namespace MonitorHooks {

	/**
	* This is called at monitor startup, before any threads are started and
	* before data capture and decoding begins.
	*/
	void beforeStartRun  (const DAQData &data);

	/**
	* This is called after the data capture and decoding threads are started.
	*/
	void startedRun      (const DAQData &data);
	
	/**
	* This is called at the very end of a run, after all data capture and
	* decoding is finished and their respective threads joined. At this
	* point, DAQData will be in its final state.
	*/
	void finishedRun     (const DAQData &data);
	
	/**
	* This is called at the beginning of each decode loop, before captured
	* data is decoded and aggregated.
	*/
	void beforeUpdateData(const DAQData &data);
	
	/**
	* This is called at the end of each decode loop, after captured data
	* is decoded and aggregated.
	*/
	void updatedData     (const DAQData &data);

}