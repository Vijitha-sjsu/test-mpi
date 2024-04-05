#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

DataLoader::DataLoader(const std::string& filename) : filename(filename) {}

std::vector<std::string> DataLoader::loadData() {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return lines;
    }

    // Read and store headers
    std::string row;
    if (getline(file, row)) {
        std::istringstream headerStream(row);
        std::string header;
        while (getline(headerStream, header, ',')) {
            // Assuming we just want to print headers or store them
            std::cout << "Header: " << header << std::endl;
        }
    }

    // Store all lines for processing
    while (getline(file, row)) {
        lines.push_back(row);
    }

    file.close(); // Close the file as it's no longer needed for direct reading

    // If needed, we could call imputeMissingValues() and removeDuplicates() here,
    // but those methods would need to be defined and implemented appropriately.

    return lines;
}
