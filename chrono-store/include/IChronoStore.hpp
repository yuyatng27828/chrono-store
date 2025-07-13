#pragma once
#include <vector>
#include <string>
#include "TickData.hpp"

class IChronoStore
{
public:
    virtual ~IChronoStore() = default;
    virtual void ingest(const std::vector<TickData> &ticks) = 0;
    virtual std::vector<TickData> query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const = 0;
};
