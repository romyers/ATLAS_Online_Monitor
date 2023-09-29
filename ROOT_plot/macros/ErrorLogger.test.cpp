#include "macros/ErrorLogger.cpp"

bool areEqual(const string &testName, size_t a, size_t b) {

	if(a == b) return true;

	cout << "FAIL -- " << testName << endl;
	cout << "\tValue " << a << " is not value " << b << endl;

	return false;

}

void ErrorLogger_test() {

	ErrorLogger &logger = *ErrorLogger::getInstance();

	areEqual("Empty logger count", logger.countErrors(), 0);
	areEqual("Empty logger count with argument", logger.countErrors("test"), 0);

	logger.logError("Test");
	logger.logError("Test", "Type");
	logger.logError("Test", "Type2");
	logger.logError("Test", "Type");

	areEqual("Populated logger count default arg", logger.countErrors(), 4);
	areEqual("Populated logger count type", logger.countErrors("Type"), 2);
	areEqual("Populated logger count type2", logger.countErrors("Type2"), 1);
	areEqual("Populated logger count random type", logger.countErrors("rand"), 0);

}