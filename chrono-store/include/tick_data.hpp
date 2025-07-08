#pragma once
#include <string>

struct TickData
{
    std::string symbol;
    int64_t timestamp;
    double price;
    int volume;
};