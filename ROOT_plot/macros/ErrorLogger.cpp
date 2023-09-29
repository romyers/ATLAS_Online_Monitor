/**
 * @file ErrorLogger.cpp
 *
 * @brief A simple singleton error logger that stores and counts errors.
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

using namespace std;

const string EMPTY_TYPE = "";

struct ErrorData {

	ErrorData(const string &msg, const string &type) : msg(msg), type(type) {}

	const string msg;
	const string type;

};

class ErrorLogger {

public:

	ErrorLogger   (      ErrorLogger &other) = delete;
	void operator=(const ErrorLogger &other) = delete;

	void clear          (                                                   );
	void logError       (const string  &msg, const string &type = EMPTY_TYPE);
	void setOutputStream(      ostream &out                                 );

	size_t countErrors(const string &type = EMPTY_TYPE) const;

	static ErrorLogger *getInstance();

private:

	ErrorLogger();

	vector<ErrorData> errors;

	ostream *errorStream;

	static ErrorLogger *instance;

};

ErrorLogger *ErrorLogger::instance = nullptr;

ErrorLogger::ErrorLogger() : errorStream(&cerr) {}

void ErrorLogger::clear() {

	errors.clear();

}

void ErrorLogger::logError(
	const string &msg, 
	const string &type = EMPTY_TYPE
) {

	*errorStream << msg << endl;

	errors.emplace_back(msg, type);

}

void ErrorLogger::setOutputStream(ostream &out) {

	errorStream = &out;

}

size_t ErrorLogger::countErrors(const string &type = EMPTY_TYPE) const {

	if(type == EMPTY_TYPE) return errors.size();

	return count_if(
		errors.cbegin(), 
		errors.cend(), 
		[type](const ErrorData &e) {
			return e.type == type;
		}
	);

}

ErrorLogger *ErrorLogger::getInstance() {

	if(instance == nullptr) {
		instance = new ErrorLogger();
	}

	return instance;

}