#include "DataLoader.h"
#include <iostream>

int main() {
    DataLoader loader1("../data/Parking_Violations_Issued_-_Fiscal_Year_2022.csv");
    //DataLoader loader2("../data/Filtered_Parking_Violations.csv");

    auto data1 = loader1.loadData();
    //auto data2 = loader2.loadData();

    std::cout << "Data from Parking_Violations_Issued_-_Fiscal_Year_2022.csv->:" << std::endl;
    for (int i = 0; i < 5 && i < data1.size(); ++i) {
        std::cout << data1[i] << std::endl;
    }

   /*/ std::cout << "\nData from Filtered_Parking_Violations.csv:" << std::endl;
    for (int i = 0; i < 5 && i < data2.size(); ++i) {
        std::cout << data2[i] << std::endl;
    }
    */

    return 0;
}
