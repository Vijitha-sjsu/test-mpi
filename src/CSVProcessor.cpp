#include "CSVProcessor.h"
#include "Constants.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <omp.h>
#include <set>
#include <ctime>
#include <iomanip>

using namespace std;

CSVProcessor::CSVProcessor(const std::string &inputFile, const std::string &outputFile, const std::string &featureEngineeredFile, const std::string &statsFile)
    : inputFile(inputFile),
      outputFile(outputFile),
      featureEngineeredFile(featureEngineeredFile),
      statsFile(statsFile),
      totalRows(0),
      validFields(NUM_FIELDS, true) {}

void CSVProcessor::imputeMissingValues()
{

    std::vector<size_t> columnsToImpute = {/* indices of columns with missing values */};
    std::vector<double> sums(columnsToImpute.size(), 0.0);
    std::vector<int> counts(columnsToImpute.size(), 0);

// First pass: Compute sums and counts for mean calculation in a thread-safe manner
#pragma omp parallel
    {
        std::vector<double> localSums(columnsToImpute.size(), 0.0);
        std::vector<int> localCounts(columnsToImpute.size(), 0);

#pragma omp for nowait
        for (size_t i = 0; i < lines.size(); ++i)
        {
            std::istringstream stream(lines[i]);
            std::string field;
            size_t fieldIndex = 0, colIndex = 0;

            while (getline(stream, field, ','))
            {
                if (colIndex < columnsToImpute.size() && fieldIndex == columnsToImpute[colIndex])
                {
                    if (!field.empty() && field != "NA")
                    {
                        localSums[colIndex] += std::stod(field);
                        localCounts[colIndex]++;
                    }
                    colIndex++;
                }
                fieldIndex++;
            }
        }

// Combine local sums and counts into global ones
#pragma omp critical
        {
            for (size_t j = 0; j < columnsToImpute.size(); ++j)
            {
                sums[j] += localSums[j];
                counts[j] += localCounts[j];
            }
        }
    }

// Second pass: Impute missing values with mean
#pragma omp parallel for
    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::istringstream stream(lines[i]);
        std::ostringstream newLine;
        std::string field;
        size_t fieldIndex = 0, colIndex = 0;

        while (getline(stream, field, ','))
        {
            if (colIndex < columnsToImpute.size() && fieldIndex == columnsToImpute[colIndex])
            {
                if (field.empty() || field == "NA")
                {
                    double mean = counts[colIndex] > 0 ? sums[colIndex] / counts[colIndex] : 0.0;
                    newLine << mean;
                }
                else
                {
                    newLine << field;
                }
                colIndex++;
            }
            else
            {
                newLine << field;
            }

            newLine << (fieldIndex < NUM_FIELDS - 1 ? "," : "");
            fieldIndex++;
        }
#pragma omp critical
        lines[i] = newLine.str();
    }
}

void CSVProcessor::removeDuplicates()
{

    std::set<std::string> uniqueLines; // This should now be recognized with the set header included
    std::vector<std::string> newLines;

    for (const auto &line : lines)
    {
        if (uniqueLines.find(line) == uniqueLines.end())
        {
            uniqueLines.insert(line);
            newLines.push_back(line);
        }
    }

    lines.swap(newLines); // Replace the old lines with the new, deduplicated lines
}

void CSVProcessor::dateToDayAndMonth(const std::string &dateStr, std::string &dayOfWeek, std::string &month)
{
    std::tm tm = {};
    std::istringstream dateStream(dateStr);
    dateStream >> std::get_time(&tm, "%m/%d/%Y"); // Assume date is in MM/DD/YYYY format

    std::time_t t = std::mktime(&tm);
    char buffer[10];

    strftime(buffer, sizeof(buffer), "%A", std::localtime(&t));
    dayOfWeek = buffer;

    strftime(buffer, sizeof(buffer), "%B", std::localtime(&t));
    month = buffer;
}

std::string CSVProcessor::timeTo24HourFormat(const std::string &timeStr)
{
    std::string result;
    int hour = std::stoi(timeStr.substr(0, 2));
    int minute = std::stoi(timeStr.substr(2, 2));
    char meridian = timeStr.back();

    if (meridian == 'P' && hour != 12)
    {
        hour += 12;
    }
    else if (meridian == 'A' && hour == 12)
    {
        hour = 0;
    }

    std::ostringstream resultStream;
    resultStream << std::setw(2) << std::setfill('0') << hour << ":"
                 << std::setw(2) << std::setfill('0') << minute;
    result = resultStream.str();
    return result;
}
void CSVProcessor::featureEngineering()
{
    std::ofstream feOut(featureEngineeredFile, std::ios::app);
    string temp;
    if (!feOut.is_open())
    {
        std::cerr << "Error opening feature engineering output file." << std::endl;
        return;
    }

    for (const auto &line : lines)
    {
        std::istringstream stream(line);
        std::ostringstream newLine;
        std::string field;
        size_t fieldIndex = 0;
        std::string dayOfWeek, month, formattedTime;

        while (getline(stream, field, ','))
        {

            // Example feature engineering for date and time fields
            if (fieldIndex == 4)
            { // Assuming 4 is the index for Issue Date
                dateToDayAndMonth(field, dayOfWeek, month);
                newLine << dayOfWeek << "," << month; // Write the new features
            }
            else if (fieldIndex == 19)
            { // Assuming 19 is the index for Violation Time
                formattedTime = timeTo24HourFormat(field);
                newLine << formattedTime; // Write the new feature
            }
            else
            {
                newLine << field; // Copy the field as is
            }

            if (fieldIndex < NUM_FIELDS - 1)
            {
                newLine << ",";
            }
            ++fieldIndex;
        }
        cout << "new line: " << newLine.str() << endl;
        feOut << newLine.str() << "\n";
    }

    feOut.close();
}

vector<string> CSVProcessor::processFile()
{
    std::ifstream file(inputFile);
    std::ofstream out(outputFile, std::ios::trunc), statsOut(statsFile, std::ios::trunc), feOut(featureEngineeredFile, std::ios::trunc);
    std::string row;
    std::vector<std::string> headers;
    std::vector<int> globalEmptyCounts(NUM_FIELDS, 0);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << inputFile << std::endl;
        return lines;
    }

    // Read and store headers
    if (getline(file, row))
    {
        std::istringstream headerStream(row);
        std::string header;
        while (getline(headerStream, header, ','))
        {
            headers.push_back(header);
        }
    }

    // Store all lines for processing
    while (getline(file, row))
    {
        lines.push_back(row);
    }

    file.close(); // Close the file as it's no longer needed for direct reading

    imputeMissingValues(); // Call the imputeMissingValues function

    // Remove duplicate lines
    removeDuplicates();

    // Update totalRows based on the number of lines read
    totalRows = lines.size();

// Parallel processing to count empty fields
#pragma omp parallel
    {
        std::vector<int> localEmptyCounts(NUM_FIELDS, 0);
#pragma omp for nowait
        for (size_t i = 0; i < lines.size(); ++i)
        {
            std::istringstream s(lines[i]);
            std::string field;
            size_t fieldIndex = 0;
            while (getline(s, field, ',') && fieldIndex < NUM_FIELDS)
            {
                if (field.empty() || field == "NA")
                {
                    localEmptyCounts[fieldIndex]++;
                }
                ++fieldIndex;
            }
        }

#pragma omp critical
        for (size_t i = 0; i < NUM_FIELDS; ++i)
        {
            globalEmptyCounts[i] += localEmptyCounts[i];
        }
    }

    // Calculate percentage of missing data and update validFields accordingly
    for (size_t i = 0; i < NUM_FIELDS; ++i)
    {
        double percentageMissing = static_cast<double>(globalEmptyCounts[i]) / totalRows * 100;
        validFields[i] = percentageMissing < 50.0; // Mark field as valid if missing data is less than 50%
        statsOut << headers[i] << "," << globalEmptyCounts[i] << "," << percentageMissing << "%," << (validFields[i] ? "Kept" : "Removed") << "\n";
    }

    // Rewrite the file with only valid fields
    file.open(inputFile); // Re-open the file for reading
    getline(file, row);   // Skip the original header row

    // Write valid headers to the output file
    bool isFirst = true;
    for (size_t i = 0; i < NUM_FIELDS; ++i)
    {
        if (validFields[i])
        {
            if (!isFirst)
            {
                out << ",";
                feOut << ",";
            }
            else
            {
                isFirst = false;
            }
            out << headers[i];
            feOut << headers[i];
        }
    }
    out << "\n";
    feOut << "\n";
    feOut.close();

    // Filter rows based on validFields
    while (getline(file, row))
    {
        std::istringstream s(row);
        std::string field;
        size_t fieldIndex = 0;
        std::string newRow;
        isFirst = true;
        while (getline(s, field, ',') && fieldIndex < NUM_FIELDS)
        {
            if (validFields[fieldIndex])
            {
                if (!isFirst)
                {
                    newRow += ",";
                }
                else
                {
                    isFirst = false;
                }
                newRow += field;
            }
            ++fieldIndex;
        }
        out << newRow << "\n";
    }

    lines.clear();

    // get all new lines from the filtered file
    ifstream filteredFile(outputFile);
    while (getline(filteredFile, row))
    {
        lines.push_back(row);
    }

    // featureEngineering(); // Implement this function based on your feature engineering needs)

    // Here, for simplicity, we just copy the data to the feature engineering output
    // You should replace this with actual feature engineering logic

    std::cout << "Filtered data saved to " << outputFile << std::endl;
    std::cout << "Feature-engineered data saved to " << featureEngineeredFile << std::endl;
    std::cout << "Field statistics saved to " << statsFile << std::endl;

    return lines;
}
