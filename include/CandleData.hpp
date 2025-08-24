#pragma once
#include <string>

struct CandleData
{
    std::string symbol;
    uint64_t timestamp;
    int64_t open;
    int64_t high;
    int64_t low;
    int64_t close;
    int64_t volume;
};