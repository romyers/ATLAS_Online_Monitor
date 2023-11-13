/**
 * @file ErrorLogger.cpp
 *
 * @brief A simple threadsafe singleton error logger that stores and counts 
 * errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <mutex>

using namespace std;

const string EMPTY_TYPE = "";

enum ErrorLevel {
	DEBUG,
	WARNING,
	ERROR,
	FATAL,
	UNDEFINED
};

struct ErrorData {

	ErrorData(
		const string &msg, 
		const string &type, 
		ErrorLevel level
	) : msg(msg), type(type), level(level) {}

	const string msg;
	const string type;
	const ErrorLevel level;

};

class ErrorLogger {

public:

	ErrorLogger   (      ErrorLogger &other) = delete;
	void operator=(const ErrorLogger &other) = delete;

	void clear            (                                                        );
	void logError         (const string  &msg, const string &type, ErrorLevel level);
	void setOutputStream  (      ostream &out                                      );

	vector<ErrorData> getErrors() const;

	size_t countErrors(const string &type = EMPTY_TYPE, ErrorLevel level = UNDEFINED) const;

	static ErrorLogger &getInstance();

private:

	ErrorLogger();

	vector<ErrorData> errors;

	ostream *errorStream;

	mutable mutex errorLock;

};

ErrorLogger::ErrorLogger() : errorStream(&cerr) {}

void ErrorLogger::clear() {

	errorLock.lock();
	errors.clear();
	errorLock.unlock();

}

void ErrorLogger::logError(
	const string &msg, 
	const string &type,
	ErrorLevel level
) {

	errorLock.lock();
	*errorStream << msg << endl;
	errors.emplace_back(msg, type, level);
	errorLock.unlock();

}

void ErrorLogger::setOutputStream(ostream &out) {

	errorLock.lock();
	errorStream = &out;
	errorLock.unlock();

}

vector<ErrorData> ErrorLogger::getErrors() const {

	errorLock.lock();
	vector<ErrorData> result = errors;
	errorLock.unlock();

	return result;

}

size_t ErrorLogger::countErrors(
	const string &type = EMPTY_TYPE, 
	ErrorLevel level = UNDEFINED
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