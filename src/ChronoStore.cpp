#include "ChronoStore.hpp"
#include "TickData.hpp"

#include <algorithm>     // std::sort, std::merge, std::lower_bound, std::upper_bound
#include <string>        // std::string
#include <thread>        // std::thread
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

// ---------------- ChronoStore internal methods ---------------- //
namespace
{
    inline bool compare_tick_timestamp(const TickData &a, const TickData &b)
    {
        return a.timestamp < b.timestamp;
    }

    inline void merge_ticks(std::vector<TickData> &existing_ticks, std::vector<TickData> &new_ticks)
    {
        // Sort new ticks
        std::sort(new_ticks.begin(), new_ticks.end(), compare_tick_timestamp);

        // Create new vector for merged ticks
        std::vector<TickData> merged_ticks;
        merged_ticks.reserve(existing_ticks.size() + new_ticks.size());

        // Merge existing and new ticks
        std::merge(existing_ticks.begin(), existing_ticks.end(),
                   new_ticks.begin(), new_ticks.end(),
                   std::back_inserter(merged_ticks),
                   compare_tick_timestamp);

        // Update existing ticks with merged ticks
        existing_ticks = std::move(merged_ticks);
    }

    inline bool tick_before_time(const TickData &tick, uint64_t ts)
    {
        return tick.timestamp < ts;
    }

    inline bool time_before_tick(uint64_t ts, const TickData &tick)
    {
        return ts < tick.timestamp;
    }
}

// ---------------- ChronoStore public methods ---------------- //
void ChronoStore::ingest(const std::vector<TickData> &ticks)
{
    // Group ticks by symbol
    std::unordered_map<std::string, std::vector<TickData>> batches;
    for (const auto &tick : ticks)
    {
        batches[tick.symbol].push_back(tick);
    }

    // Launch threads to merge ticks for each symbol
    std::vector<std::thread> threads;
    threads.reserve(batches.size());

    for (auto &[symbol, batch] : batches)
    {
        threads.emplace_back([this, &symbol, &batch]()
                             {
            std::lock_guard<std::mutex> lock(symbol_mutexes_[symbol]);
            merge_ticks(ticks_by_symbol_[symbol], batch); });
    }

    // Wait for all threads to finish
    for (auto &t : threads)
    {
        t.join();
    }
}

std::pair<ChronoStore::TickIterator, ChronoStore::TickIterator>
ChronoStore::query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const
{
    // Check if the symbol exists in the store
    auto it = ticks_by_symbol_.find(symbol);
    if (it == ticks_by_symbol_.end())
    {
        return {{}, {}};
    }

    // Get the ticks for that symbol
    const auto &ticks = it->second;

    // Find the range of ticks within the specified time range
    auto start_it = std::lower_bound(ticks.begin(), ticks.end(), start_time, tick_before_time);
    auto end_it = std::upper_bound(ticks.begin(), ticks.end(), end_time, time_before_tick);

    return {start_it, end_it};
}