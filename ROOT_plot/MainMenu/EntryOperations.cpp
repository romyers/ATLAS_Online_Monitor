#include "EntryOperations.h"

#include "DAQMonitor/DataRunOperations.h"

#include "src/ProgramControl/Terminator.h"

using namespace std;
using namespace Muon;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void EntryOperations::exitAll() {

	Terminator::getInstance().terminate();

}

void EntryOperations::stopRun() {

	DataRun::stopRun();

}

void EntryOperations::startRun() {

	DataRun::startRun();
	
}