#include "RunStats.h"

#include <string>

#include "macros/ErrorLogger.h"
#include "macros/UIFramework/UISignals.h"
#include "macros/DAQState.h"

#include "src/DataModel/DAQData.h"

using namespace std;
using namespace Muon;

// TODO: Examine this example:
//       https://root.cern/doc/master/calendar_8C.html

// TODO: See about reorganizing logic and dependencies, keeping with the
//       view vs operations pattern I've been following



void RunStats::makeConnections() {

}

RunStats::RunStats(
	const TGWindow *parent
) : TGVerticalFrame(parent, 100, 220, kVerticalFrame) {

	table = new TGHtml(this, 100, 220);
	AddFrame(table, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	makeConnections();

	update();

}

void RunStats::update() {

	DAQData &data = DAQData::getInstance();
	ErrorLogger &logger = ErrorLogger::getInstance();

	// Update the table
	data.lock();
	statsTable.setEntry("Packets Recorded", to_string(data.packetCount));
	statsTable.setEntry("Events Processed", to_string(data.totalEventCount));
	statsTable.setEntry("Empty Events", to_string(data.totalEventCount - data.processedEvents.size()));
	statsTable.setEntry("Error Count", to_string(logger.countErrors(EMPTY_TYPE, ERROR)));
	statsTable.setEntry("Warning Count", to_string(logger.countErrors(EMPTY_TYPE, WARNING)));
	statsTable.setEntry("Packets Lost", to_string(data.lostPackets));
	statsTable.setEntry("Dropped Signals", to_string(data.droppedSignals));
	statsTable.setEntry("Dropped Events", to_string(data.droppedEvents));
	data.unlock();

	string htmlString = statsTable.stringify();

	// Append the run status
	// TODO: This could go somewhere else
	State::DAQState state = State::DAQState::getState();
	if(state.tempState.runLabel != "") {

		htmlString += "<h2>";

		if(state.tempState.runStarted) {

			htmlString += string("Started Run: ") + state.tempState.runLabel;

		} else {

			htmlString += string("Stopped Run: ") + state.tempState.runLabel;

		}

		htmlString += "</h2>";

	}

	// Package the html into the html GUI element
	char *htmlText = new char[htmlString.size() + 1];
	try{

		strcpy(htmlText, htmlString.data());

		table->Clear();

		table->ParseText(htmlText);

		table->Layout();

	} catch (exception &e) {

		delete[] htmlText;
		htmlText = nullptr;

		throw e;

	}

	delete[] htmlText;
	htmlText = nullptr;

}

void RunStats::breakConnections() {

	UISignalBus::getInstance().Disconnect("onUpdate()", this, "update()");

}

void RunStats::teardown() {

	breakConnections();

}