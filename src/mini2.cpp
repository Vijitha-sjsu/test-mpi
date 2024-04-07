#include "CSVProcessor.h"
#include "Analyzer.h"
#include <iostream>
#include <mpi.h>

using namespace std;
int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    CSVProcessor processor("../data/dataset.csv", "../processedfiles/Filtered_Parking_Violations.csv", "../processedfiles/feature_engineered_data.csv", "../processedfiles/processed_stats.txt");

    vector<string> processedLines = processor.processFile();

    // Analyzer analyzer(processedLines);
    // analyzer.aggregateData();
    // analyzer.showTopTenPrecincts();

    MPI_Finalize();

    return 0;
}
