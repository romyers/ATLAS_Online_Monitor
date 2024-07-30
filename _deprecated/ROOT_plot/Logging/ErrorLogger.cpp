#include "ErrorLogger.h"

#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

ErrorData::ErrorData(
	const string &message, 
	const string &type, 
	ErrorLevel level
) : msg(message), 
	type(type), 
	level(level), 
	timestamp(chrono::system_clock::to_time_t(chrono::system_clock::now())) 
{}

string ErrorData::stringify() const {

	string time = string(ctime(&timestamp));

	string errorLevel = "";

	// I use a switch statement instead of defining the enums as strings for
	switch(level) {

	case DEBUG:
		errorLevel = "DEBUG";
		break;

	case INFO:
		errorLevel = "INFO";
		break;

	case WARNING:
		errorLevel = "WARNING";
		break;

	case ERROR:
		errorLevel = "ERROR";
		break;

	case CRITICAL:
		errorLevel = "CRITICAL";
		break;

	default:
		errorLevel = "";
		break;

	}

	// ctime adds a newline character we don't want. So we trim it from the end
	// of the string.
	
	return time.substr(0, time.length() - 1) 
		+ string(": ") 
		+ errorLevel 
		+ " -- " 
		+ msg;

}

ErrorLogger::ErrorLogger() {

	errorStreams.push_back(&cerr);

}

void ErrorLogger::clear() {

	errorLock.lock();
	errors.clear();
	errorLock.unlock();

}

void ErrorLogger::disconnectStreams() {

	errorLock.lock();
	errorStreams.clear();
	errorStreams.push_back(&cerr);
	errorLock.unlock();

}

void ErrorLogger::logError(
	const string &msg, 
	const string &type,
	ErrorLevel level
) {

	errorLock.lock();
	errors.emplace_back(msg, type, level);
	for(ostream *stream : errorStreams) {
		*stream << errors.back().stringify() << endl;
	}
	errorLock.unlock();

}

void ErrorLogger::addOutputStream(ostream &out) {

	errorLock.lock();
	errorStreams.push_back(&out);
	errorLock.unlock();

}

vector<ErrorData> ErrorLogger::getErrors() const {

	errorLock.lock();
	vector<ErrorData> result = errors;
	errorLock.unlock();

	return result;

}

size_t ErrorLogger::countErrors(
	const string &type, 
	ErrorLevel level
) const {

	size_t val;

	errorLock.lock();

	if(type == EMPTY_TYPE && level == UNDEFINED) {

		val = errors.size();

	} else {

		val = count_if(
			errors.cbegin(),
			errors.cend(),
			[&type, level](const ErrorData &e) {

				if(type != EMPTY_TYPE) {

					if(e.type != type) return false;

				}

				if(level != UNDEFINED) {

					if(e.level != level) return false;

				}

				return true;
				
			}
		);

	}

	errorLock.unlock();

	return val;

}

ErrorLogger &ErrorLogger::getInstance() {

	static ErrorLogger instance;

	return instance;

}