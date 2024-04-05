#ifndef CSVPROCESSOR_H
#define CSVPROCESSOR_H

#include <string>
#include <vector>

class CSVProcessor
{
public:
    CSVProcessor(const std::string &inputFile, const std::string &outputFile, const std::string &featureEngineeredFile, const std::string &statsFile);
    void processFile();
    void imputeMissingValues();
    void removeDuplicates();
    void featureEngineering();
    static constexpr size_t NUM_FIELDS = 43;

private:
    std::string inputFile;
    std::string outputFile;
    std::string featureEngineeredFile;
    std::string statsFile;
    std::vector<std::string> lines;
    std::vector<bool> validFields;
    size_t totalRows;

    // Declare the new methods as private member functions
    void dateToDayAndMonth(const std::string &dateStr, std::string &dayOfWeek, std::string &month);
    std::string timeTo24HourFormat(const std::string &timeStr);

    std::vector<int> countEmptyFieldsPerColumn(const std::vector<std::string> &chunk);
    bool isValidRow(const std::string &row);
};

#endif // CSVPROCESSOR_H
