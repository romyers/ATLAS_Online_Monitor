/**
 * @file ErrorLogger.cpp
 *
 * @brief A simple threadsafe singleton error logger that stores and counts 
 * errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include "ErrorLogger.h"

#include <stdio.h>
#include <algorithm>

using namespace std;

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

	// TODO: Consider saving the error level with the error

	errorLock.lock();
	for(ostream *stream : errorStreams) {
		*stream << msg << endl;
	}
	errors.emplace_back(msg, type, level);
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