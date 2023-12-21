/**
 * @file PairTable.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <utility>
#include <string>

// TODO: Options for table formatting, e.g. background and font color
class PairTable {

public:

	PairTable();

	void setEntry(const std::string &key, const std::string &value);

	std::string stringify() const;

private:

	std::vector<std::pair<std::string, std::string>> tableRows;

};