#include "ChronoStore.hpp"
#include "TickData.hpp"

#include <algorithm>     // std::sort, std::merge, std::lower_bound, std::upper_bound
#include <iostream>      // std::cout
#include <string>        // std::string
#include <thread>        // std::thread
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

// int ChronoStore::intern_symbol(const std::string &symbol)
// {
//     auto it = symbol_to_id_.find(symbol);
//     if (it != symbol_to_id_.end())
//         return it->second;

//     int id = static_cast<int>(id_to_symbol_.size());
//     symbol_to_id_[symbol] = id;
//     id_to_symbol_.push_back(symbol);
//     return id;
// }

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
}