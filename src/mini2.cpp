#include "CSVProcessor.h"
#include "Analyzer.h"
#include <iostream>

using namespace std;
int main()
{
    CSVProcessor processor("../data/small.csv", "../processedfiles/Filtered_Parking_Violations.csv", "../processedfiles/feature_engineered_data.csv", "../processedfiles/processed_stats.txt");

    vector<string> processedLines = processor.processFile();

    Analyzer analyzer(processedLines);
    analyzer.aggregateData();
    analyzer.showTopTenPrecincts();

    return 0;
}
