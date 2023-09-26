/**
 * @file monitorFile.cpp
 *
 * @brief Defines a Root macro that polls an input file for modifications and
 * calls a specified handler whenever the input file is appended to.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

// TODO: Make sure an integral unit of data is written before the monitor reads 
//       the new data. Perhaps we can do this by controlling how EthernetCap 
//       flushes its output? Or perhaps this is fine out of the box?
//         -- Examine: https://stackoverflow.com/a/30133368
//              >> Describes a way to lock files to prevent data races

// TODO: Consider using inotify

#include <fstream>
#include <functional>

// In case this needs to run on a Windows machine:
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// The amount of time the monitor waits to poll for file changes
const double POLL_INTERVAl = 0.1; // seconds

/**
 * Returns true if 'in' is currently at the end of its file, and false 
 * otherwise.
 * 
 * @param in The ifstream to check.
 */
bool hasUnreadContent(ifstream &in) {

	// NOTE: in.peek() sets the fail bit if we hit EOF. We will need to clear
	//       it every time.
	return in.peek() != EOF;

}

/**
 * A default handler for appends to the input file, writing the number of times
 * the file has been appended to since the monitor was initiated to standard 
 * output.
 * 
 * @param in An ifstream referring to the input file.
 */
void handleFileAppend(ifstream &in) {

	static int numModifications = 0;

	++numModifications;

	cout << "File has been modified " 
		 << numModifications 
		 << " times." 
		 << endl;

}

/**
 * Sets the seeker head of an ifstream to the end of its file.
 * 
 * @param in The ifstream to modify.
 */
void jumpToEndOfFile(ifstream &in) {

    in.seekg(0, in.end);

}

/**
 * Polls the file referred to by 'filename' for append operations and
 * calls 'appendHandler' with an ifstream to the modified file whenever an 
 * append is found to have occurred. The polling interval can be set at
 * monitorFile.cpp:29.
 *
 * The ifstream reference given to appendHandler will have its seeker head
 * set to the beginning of the newly appended data. Any seeker positions
 * set by the handler will not be maintained between calls, and the seeker
 * will need to be manually set to another position if one is required.
 * 
 * If no append handler is specified, the monitor will write the number of 
 * times the file has been appended to since the monitor was initiated to 
 * standard output.
 * 
 * The append handler will not be called when the monitor is first run
 * regardless of file content. The monitor will wait for an append operation
 * before calling 'appendHandler'.
 * 
 * Behavior is not defined for modification or deletion of existing content
 * in the file represented by filename.
 * 
 * @param filename The name of the file to be monitored.
 * @param appendHandler A functional with void return type accepting one
 * ifstream& argument that will be called whenever the file represented by
 * 'filename' undergoes an append operation.
 * @param binaryMode Whether the monitor should treat the input file as a
 * binary file. True by default.
 */
int MonitorFile(
	const string &filename, 
	function<void(ifstream&)> appendHandler = handleFileAppend,
    bool binaryMode = true
) {

    ios_base::openmode mode = binaryMode? ios::binary : ios::in;

	ifstream in(filename);
    jumpToEndOfFile(in);

	while(true) {

		if(hasUnreadContent(in)) {

			appendHandler(in);
            jumpToEndOfFile(in);

		} else {

			in.clear();
			sleep(POLL_INTERVAl);

		}

	}

	return 0;

}