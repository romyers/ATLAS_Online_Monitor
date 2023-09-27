#include <fstream>
#include <stdio.h>

#include "FileAppendMonitor.h"

using namespace std;

void FileAppendMonitor_test() {

	int testFails = 0;

	ofstream out("TEMP.test.txt", ofstream::app);

	out << "Initialized Output File" << endl;

	FileAppendMonitor monitor("TEMP.test.txt");

	if(monitor.hasAppend()) {
		cout << "FAIL -- FileAppendMonitor is clear on initialization." << endl;
		++testFails;
	}

	out << "Appended to output file" << endl;

	if(!monitor.hasAppend()) {
		cout << "FAIL -- FileAppendMonitor responds to file changes." << endl;
		++testFails;
	}

	monitor.clearAppend();

	if(monitor.hasAppend()) {
		cout << "FAIL -- FileAppendMonitor can be cleared." << endl;
		++testFails;
	}

	if(testFails > 0) {
		cout << "Failed " << testFails << " test cases." << endl;
	} else {
		cout << "All test cases passed." << endl;
	}

	remove("TEMP.test.txt");

}