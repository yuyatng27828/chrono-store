#pragma once
#include <string>

struct TickData
{
    std::string symbol;
    uint64_t timestamp;
    int64_t price;
    int64_t volume;
};