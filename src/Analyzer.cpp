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

	std::ofstream outFile("../processedfiles/hotspots.csv");

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