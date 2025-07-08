#include "chrono_store.hpp"

void ChronoStore::ingest(const TickData &tick)
{
    data_store_.push_back(tick);
}

std::vector<TickData> ChronoStore::query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const
{
    std::vector<TickData> result;
    for (const auto &tick : data_store_)
    {
        if (tick.timestamp >= start_time && tick.timestamp <= end_time && tick.symbol == symbol)
        {
            result.push_back(tick);
        }
    }
    return result;
}