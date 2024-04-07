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
    CSVProcessor processor("../data/dataset.csv", "../processedfiles/Filtered_Parking_Violations.csv", "../processedfiles/feature_engineered_data.csv", "../processedfiles/processed_stats.txt");

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

    return 0;
}
