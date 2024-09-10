#include "SettingsManager.h"

using namespace DAQ;

using std::string;

#include <fstream>

// The delimiter between keys and values in the settings file.
const char DELIMITER = ':';

SettingsManager::SettingsManager(const string &path) : path(path) {

	refresh();

}

bool SettingsManager::has(const string &key) {

	return data.find(key) != data.end();

}

bool SettingsManager::setDefault(const string &key, const string &value) {

	// Do nothing if the key already exists
	if (has(key)) {
		return false;
	}

	data[key] = value;

	return true;

}

string &SettingsManager::at(const string &key) {

	return data.at(key);

}

string &SettingsManager::operator[](const string &key) {

	return data[key];

}

bool SettingsManager::save() {

	std::ofstream out(path, std::ofstream::trunc);

	for (auto &pair : data) {
		out << pair.first << DELIMITER << pair.second << std::endl;
	}

	out << std::flush;

	out.close();

	// If something went wrong, return false
	if(!out) return false;

	return true;

}

bool SettingsManager::refresh() {

	std::ifstream in(path);

	// If it isn't open, do nothing. All the settings will be defaulted
	// and the file will be rewritten later.
	if(in.is_open()) {

		data.clear();

		string line;

		while (std::getline(in, line)) {

			size_t pos = line.find(DELIMITER);

			// Skip malformed lines
			if (pos == string::npos) {
				continue;
			}

			string key = line.substr(0, pos);
			string value = line.substr(pos + 1);

			// If a setting appears twice, the last one wins.
			// The next write operation will remove one of the duplicates.
			data[key] = value;

		}

		return true;

	}

	return false;

}