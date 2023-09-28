/**
 * @file ErrorLogger.cpp
 *
 * @brief A simple singleton error logger that stores and counts errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

using namespace std;

struct ErrorData {

	ErrorData(const string &msg) : msg(msg) {}

	string msg;

};

class ErrorLogger {

public:

	ErrorLogger   (      ErrorLogger &other) = delete;
	void operator=(const ErrorLogger &other) = delete;

	void clear          ();
	void logError       (const string &msg);
	void setOutputStream(ostream &out);

	size_t countErrors();

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

void ErrorLogger::logError(const string &msg) {

	*errorStream << msg << endl;

	errors.emplace_back(msg);

}

void ErrorLogger::setOutputStream(ostream &out) {

	errorStream = &out;

}

size_t ErrorLogger::countErrors() {

	return errors.size();

}

ErrorLogger *ErrorLogger::getInstance() {

	if(instance == nullptr) {
		instance = new ErrorLogger();
	}

	return instance;

}