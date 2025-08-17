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
    for (const auto &tick : ticks)
    {
        auto &buffer = ticks_by_symbol_.try_emplace(
                                           tick.symbol, RingBuffer<TickData>(capacity_per_symbol_))
                           .first->second;

        buffer.push(tick);
    }
}

// std::pair<ChronoStore::TickIterator, ChronoStore::TickIterator>
// ChronoStore::query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const
// {
//     // Check if the symbol exists in the store
//     auto it = ticks_by_symbol_.find(symbol);
//     if (it == ticks_by_symbol_.end())
//     {
//         return {{}, {}};
//     }

//     // Get the ticks for that symbol
//     const auto &ticks = it->second;

//     // Find the range of ticks within the specified time range
//     auto start_it = std::lower_bound(ticks.begin(), ticks.end(), start_time, tick_before_time);
//     auto end_it = std::upper_bound(ticks.begin(), ticks.end(), end_time, time_before_tick);

//     return {start_it, end_it};
// }