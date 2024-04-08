#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "ViolationData.h"

using namespace std;

class Analyzer {
public:
    Analyzer(vector<string> &lines);

    void aggregateData();
    void showTopTenPrecincts();
    void analyzeAndPlotCommonViolations();
    void analyzeViolationsByTimeOfDay();
    void analyzeViolationsByMonth();
    void plotData(const vector<double>& values, const vector<string>& labels, const string& title, const string& xlabel, const string& ylabel);

private:
    vector<string> lines;
    PrecinctDataMap violationHotspots;
    unordered_map<string, int> timeOfDayCounts;
    unordered_map<string, int> monthCounts;
};

#endif // ANALYZER_H
