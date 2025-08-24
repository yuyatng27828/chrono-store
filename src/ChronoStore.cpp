#include "ChronoStore.hpp"
#include "TickData.hpp"

#include <algorithm>     // std::sort, std::merge, std::lower_bound, std::upper_bound
#include <iostream>      // std::cout
#include <string>        // std::string
#include <thread>        // std::thread
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

void ChronoStore::ingest(const std::vector<TickData> &ticks)
{
    std::unordered_map<std::string, std::vector<TickData>> buckets;

    for (const auto &tick : ticks)
    {
        auto &bucket = buckets[tick.symbol];
        bucket.push_back(tick);
    }

    for (const auto &[symbol, ticks] : buckets)
    {
        auto it = ticks_by_symbol_.find(symbol);
        if (it == ticks_by_symbol_.end())
            it = ticks_by_symbol_.emplace(symbol, RingBuffer<TickData>(capacity_per_symbol_)).first;
        it->second.push(ticks);
    }
};

ChronoStore::RingBufferSpanRange ChronoStore::query_last_n(const std::string &symbol, size_t n) const
{
    auto it = ticks_by_symbol_.find(symbol);
    if (it == ticks_by_symbol_.end())
        return {{}, {}};
    return it->second.get_last_n(n);
}

ChronoStore::RingBufferSpanRange ChronoStore::query_last_t(const std::string &symbol, std::chrono::milliseconds duration) const
{
    auto it = ticks_by_symbol_.find(symbol);
    if (it == ticks_by_symbol_.end())
        return {{}, {}};
    return it->second.get_last_t(duration);
}