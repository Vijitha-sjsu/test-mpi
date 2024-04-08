#include "CSVProcessor.h"
#include "Analyzer.h"
#include <iostream>
#include <mpi.h>
#include <fstream>

using namespace std;
int main(int argc, char *argv[])
{

    fstream filteredFile("../processedfiles/Filtered_Parking_Violations.csv");

    MPI_Init(&argc, &argv);
    CSVProcessor processor("/Users/vijithagunta/eclipse-workspace/new-mini2/Mini-Project-2-CMPE275/data/dataset.csv", "/Users/vijithagunta/eclipse-workspace/new-mini2/Mini-Project-2-CMPE275/processedfiles/Filtered_Parking_Violations.csv", "../processedfiles/feature_engineered_data.csv", "/Users/vijithagunta/eclipse-workspace/new-mini2/Mini-Project-2-CMPE275/processedfiles/processed_stats.txt");

    // vector<string> processedLines = processor.processFile();
    processor.processFile();
    MPI_Finalize();

    vector<string> processedLines;
    string row;
    while (getline(filteredFile, row))
    {
        processedLines.push_back(row);
    }
    cout << "Got all: " << processedLines.size() << endl;

    Analyzer analyzer(processedLines);
    analyzer.aggregateData();
    analyzer.showTopTenPrecincts();
    analyzer.analyzeAndPlotCommonViolations();

    return 0;
}
