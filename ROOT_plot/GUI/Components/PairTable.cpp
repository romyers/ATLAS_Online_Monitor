#include "PairTable.h"

#include <algorithm>

using namespace std;

PairTable::PairTable() {}

void PairTable::setEntry(const string &key, const string &value) {

	auto iter = find_if(
		tableRows.begin(), 
		tableRows.end(), 
		[&key](const pair<string, string> &row) {

			return row.first == key;

		}
	);

	if(iter == tableRows.end()) {

		tableRows.push_back(pair<string, string>({key, value}));
		return;

	}

	iter->second = value;

}

string PairTable::stringify() const {

	string result = "<TABLE width=100% border=1";

		result += "<TBODY>";

		for(const pair<string, string> &row : tableRows) {

			result += "<TR>";

				result += "<TH width=30% align=left>";
					result += row.first;
				result += "</TH>";

				result += "<TH width=30% align=right>";
					result += row.second;
				result += "</TH>";

			result += "</TR>";

		}

		result += "</TBODY>";

	result += "</TABLE>";

	return result;

}