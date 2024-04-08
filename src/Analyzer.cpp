#include "Analyzer.h"
#include "Constants.h"
#include "ViolationData.h"
#include "omp.h"
#include <sstream>
#include <algorithm>
#include <matplot/matplot.h>

using namespace std;

Analyzer::Analyzer(vector<string> &lines) : lines(lines) {}

void Analyzer::aggregateData()
{
#pragma omp parallel for
	for (int i = 1; i < lines.size(); i++)
	{
		int currentIndex = 0;
		std::istringstream iss(lines[i]);
		std::string field;

		while (currentIndex != static_cast<int>(ColumnIndexes::violationCode))
		{
			std::getline(iss, field, ','); // Skip to the violation code
			currentIndex++;
		}
		std::string violationCode = field;

		while (currentIndex != static_cast<int>(ColumnIndexes::violationPrecinct))
		{
			std::getline(iss, field, ','); // Skip to the precinct
			currentIndex++;
		}
		std::string precinct = field;

		while (currentIndex != static_cast<int>(ColumnIndexes::violationInFrontOrOpposite))
		{
			std::getline(iss, field, ','); // Skip to the violation in front or opposite
			currentIndex++;
		}
		std::string violationInFrontOrOpposite = field;

		while (currentIndex != static_cast<int>(ColumnIndexes::streetName))
		{
			std::getline(iss, field, ','); // Skip to the street name
			currentIndex++;
		}
		std::string streetName = field;

// Aggregate data (this is simplified, you may need more complex logic)
#pragma omp critical
		{
			Violation violation;
			violation.streetName = streetName;
			violation.violationCode = violationCode;
			violation.violationInFrontOfOrOpposite = violationInFrontOrOpposite;

			violationHotspots[precinct].violations.push_back(violation);
		}
	}
}

void Analyzer::showTopTenPrecincts()
{
	std::vector<std::pair<std::string, int>> precinctCounts;

	for (const auto &entry : violationHotspots)
	{
		precinctCounts.push_back(make_pair(entry.first, entry.second.violations.size()));
	}

	std::sort(precinctCounts.begin(), precinctCounts.end(), [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
			  { return a.second > b.second; });

	std::ofstream outFile("/Users/vijithagunta/eclipse-workspace/new-mini2/Mini-Project-2-CMPE275/processedfiles/hotspots.csv");

	// Check if the file stream is open
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file for writing" << std::endl;
		return;
	}

	// Write the header
	outFile << "Precinct, Violation Count\n";

	// Write the data
	for (const auto &[precinct, count] : precinctCounts)
	{
		outFile << precinct << ", " << count << "\n";
	}

	outFile.close();
	// Extract the top 10 precincts for visualization
	std::vector<std::string> precincts;
	std::vector<double> counts;
	for (int i = 0; i < 10 && i < precinctCounts.size(); ++i)
	{
		precincts.push_back(precinctCounts[i].first);
		counts.push_back(static_cast<double>(precinctCounts[i].second));
	}

	auto bars = matplot::bar(counts);

	// Set the x-tick labels at positions 0, 1, 2, ..., N-1
	matplot::xticks(matplot::iota(0, counts.size() - 1));
	matplot::xticklabels(precincts);
	matplot::xlabel("Precinct");
	matplot::ylabel("Violation Count");
	matplot::title("Top 10 Precincts by Violation Count");

	// Display the plot
	matplot::show();
}

void Analyzer::analyzeAndPlotCommonViolations() {
    // A map to hold violation codes and their counts
    std::unordered_map<std::string, int> violationCounts;

    // Aggregate violation codes
    #pragma omp parallel for
    for (size_t i = 0; i < lines.size(); ++i) {
        std::istringstream iss(lines[i]);
        std::string field;

        // Assuming Violation Code is the 5th field
        for (int j = 0; j < 5; ++j) {
            std::getline(iss, field, ',');
        }

        #pragma omp critical
        violationCounts[field]++;
    }

    // Prepare data for plotting
    std::vector<std::string> codes;
    std::vector<double> counts;
    for (const auto& vc : violationCounts) {
        codes.push_back(vc.first);
        counts.push_back(static_cast<double>(vc.second));
    }

    // Sort by count descending
    std::vector<int> idx(counts.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&counts](int i1, int i2) {return counts[i1] > counts[i2];});

    // Take top 10 (or fewer, if there aren't that many)
    size_t topN = std::min<size_t>(10, counts.size());
    std::vector<std::string> topCodes(topN);
    std::vector<double> topCounts(topN);
    for (size_t i = 0; i < topN; ++i) {
        topCodes[i] = codes[idx[i]];
        topCounts[i] = counts[idx[i]];
    }

    // Plotting
    auto fig = matplot::bar(topCounts);
    matplot::xticks(matplot::iota(0, topN - 1));
    matplot::xticklabels(topCodes);
    matplot::xlabel("Violation Code");
    matplot::ylabel("Count");
    matplot::title("Top 10 Most Common Violations");

    matplot::show();
}
