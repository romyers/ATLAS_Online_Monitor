/**
 * @file Histogram.cpp
 *
 * @brief TODO: Write
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <utility>

using namespace std;

struct HistogramMetadata {

	int panelIndex;

};

class HistogramPanel {

public:

	HistogramPanel();

private:

	vector<pair<TH1*, HistogramMetadata>> histograms;

	TCanvas *canvas;

};

HistogramPanel::HistogramPanel() {}