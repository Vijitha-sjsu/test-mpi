#ifndef ANALYZER
#define ANALYZER

#include <iostream>
#include <string>
#include <vector>
#include "ViolationData.h"

using namespace std;

class Analyzer
{

public:
	Analyzer(std::vector<string> &lines);
	void aggregateData();
	void showTopTenPrecincts();

private:
	vector<string> lines;
	PrecinctDataMap violationHotspots;
};

#endif