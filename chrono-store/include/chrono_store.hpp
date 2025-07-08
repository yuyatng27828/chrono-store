#pragma once
#include <vector>
#include "tick_data.hpp"

class ChronoStore
{
public:
    void ingest(const TickData &tick);
    std::vector<TickData> query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const;

private:
    std::vector<TickData> data_store_;
};