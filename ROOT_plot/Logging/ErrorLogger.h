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
#include <time.h>

const std::string EMPTY_TYPE = "";

enum ErrorLevel {

	/**
	 * Marker for debug information that isn't necessarily erroneous.
	 */
	DEBUG,

	/**
	 * Marker for any nonerroneous information suitable for logging in a 
	 * production build.
	 */
	INFO,

	/**
	 * Marker for warnings that do not interfere with the program.
	 */
	WARNING,

	/**
	 * Marker for errors that may interfere with the program.
	 */
	ERROR,

	/**
	 * Marker for critical errors that the program cannot recover from. 
     * CRITICAL should be reserved for errors that will crash the program.
	 */
	CRITICAL,

	/**
	 * Marker used in filtering errors to signify that you do not wish to
	 * filter by error level. Errors should not themselves be marked
	 * UNDEFINED.
	 */
	UNDEFINED
	
};

struct ErrorData {

	ErrorData(
		const std::string &message, 
		const std::string &type, 
		ErrorLevel level
	);

	const std::string msg;
	const std::string type;
	const ErrorLevel level;

	const time_t timestamp;

	std::string stringify() const;

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