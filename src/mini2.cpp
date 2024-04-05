#include "CSVProcessor.h"

int main() {
    CSVProcessor processor("../data/Parking_Violations_Issued_-_Fiscal_Year_2022.csv", "Filtered_Parking_Violations.csv");

    processor.processFile();

    return 0;
}
