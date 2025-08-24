#include "CandleData.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // for std::find_first_not_of / find_last_not_of

inline std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\n\r\"");
    size_t end = s.find_last_not_of(" \t\n\r\"");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<CandleData> load_candles_from_csv(const std::string &filepath, const std::string &symbol)
{
    std::vector<CandleData> candles;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open CSV file: " << filepath << std::endl;
        return candles;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        std::cerr << "CSV file is empty: " << filepath << std::endl;
        return candles;
    }

    std::cout << "CSV header: " << line << std::endl;

    size_t line_number = 1;
    while (std::getline(file, line))
    {
        ++line_number;
        std::stringstream ss(line);
        std::string field;
        CandleData c;
        c.symbol = symbol;

        try
        {
            // Symbol
            std::getline(ss, field, ',');
            c.symbol = trim(field);

            // Timestamp
            std::getline(ss, field, ',');
            c.timestamp = std::stoull(trim(field));

            // Open
            std::getline(ss, field, ',');
            c.open = std::stoll(trim(field));

            // High
            std::getline(ss, field, ',');
            c.high = std::stoll(trim(field));

            // Low
            std::getline(ss, field, ',');
            c.low = std::stoll(trim(field));

            // Close
            std::getline(ss, field, ',');
            c.close = std::stoll(trim(field));

            // Volume
            std::getline(ss, field, ',');
            c.volume = std::stoll(trim(field));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing line " << line_number << ": " << e.what() << "\n"
                      << "Line content: " << line << std::endl;
            continue; // skip malformed line
        }

        candles.push_back(c);
    }

    std::cout << "Loaded " << candles.size() << " candles from " << filepath << std::endl;
    return candles;
}