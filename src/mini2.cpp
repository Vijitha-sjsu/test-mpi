#include "CSVProcessor.h"

int main()
{
    // CSVProcessor processor("../data/Parking_Violations_Issued_-_Fiscal_Year_2022.csv", "Filtered_Parking_Violations.csv");
    CSVProcessor processor("../data/small.csv", "../processedfiles/Filtered_Parking_Violations.csv", "../processedfiles/feature_engineered_data.csv", "../processedfiles/processed_stats.txt");

    processor.processFile();

    return 0;
}
