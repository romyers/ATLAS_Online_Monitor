/**
 * @file ErrorLogger.h
 *
 * @brief A simple threadsafe singleton error logger that stores and counts 
 * errors.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <mutex>

const std::string EMPTY_TYPE = "";

enum ErrorLevel {
	DEBUG,
	WARNING,
	ERROR,
	FATAL,
	UNDEFINED
};

struct ErrorData {

	ErrorData(
		const std::string &msg, 
		const std::string &type, 
		ErrorLevel level
	) : msg(msg), type(type), level(level) {}

	const std::string msg;
	const std::string type;
	const ErrorLevel level;

};

class ErrorLogger {

public:

	ErrorLogger   (      ErrorLogger &other) = delete;
	void operator=(const ErrorLogger &other) = delete;

	void clear            (                                                                  );
	void disconnectStreams(                                                                  );
	void logError         (const std::string  &msg, const std::string &type, ErrorLevel level);
	void addOutputStream  (      std::ostream &out                                           );

	std::vector<ErrorData> getErrors() const;

	size_t countErrors(const std::string &type = EMPTY_TYPE, ErrorLevel level = UNDEFINED) const;

	static ErrorLogger &getInstance();

private:

	ErrorLogger();

	std::vector<ErrorData> errors;
	std::vector<std::ostream*> errorStreams;

	mutable std::mutex errorLock;

};