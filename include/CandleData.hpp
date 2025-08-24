#pragma once
#include <string>

struct CandleData
{
    std::string symbol;
    uint64_t timestamp;
    double open;
    double high;
    double low;
    double close;
    int64_t volume;
};