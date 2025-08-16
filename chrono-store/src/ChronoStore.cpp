#include "ChronoStore.hpp"

void ChronoStore::ingest(const std::vector<TickData> &ticks)
{

    // Map ticks to their respective symbols
    std::unordered_map<std::string, std::vector<TickData>> batch_per_symbol;
    for (const auto &tick : ticks)
    {
        batch_per_symbol[tick.symbol].push_back(tick);
    }

    // Define a lambda to compare timestamps
    auto timestamp_cmp = [](const TickData &a, const TickData &b)
    {
        return a.timestamp < b.timestamp;
    };

    // Iterate over each symbol's new batch
    for (auto &[symbol, batch] : batch_per_symbol)
    {
        // Sort the new batch of ticks for the symbol
        std::sort(batch.begin(), batch.end(), timestamp_cmp);

        // Create a new vector for the merged ticks
        auto &original_symbol_ticks = ticks_by_symbol_[symbol];
        std::vector<TickData> merged_ticks;
        merged_ticks.reserve(original_symbol_ticks.size() + batch.size());

        // Merge the existing ticks with the new batch
        std::merge(original_symbol_ticks.begin(), original_symbol_ticks.end(),
                   batch.begin(), batch.end(),
                   std::back_inserter(merged_ticks),
                   timestamp_cmp);

        // Store the merged ticks back into the map
        ticks_by_symbol_[symbol] = std::move(merged_ticks);
    }
}

std::vector<TickData> ChronoStore::query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const
{
    std::vector<TickData> result;

    // Check if the symbol exists in the store
    auto it = ticks_by_symbol_.find(symbol);
    if (it == ticks_by_symbol_.end())
    {
        return result;
    }

    // Get the ticks for that symbol
    const auto &ticks = it->second;

    // Find the range of ticks within the specified time range
    auto start_it = std::lower_bound(ticks.begin(), ticks.end(), start_time,
                                     [](const auto &tick, auto ts)
                                     { return tick.timestamp < ts; });

    auto end_it = std::upper_bound(ticks.begin(), ticks.end(), end_time,
                                   [](auto ts, const auto &tick)
                                   { return ts < tick.timestamp; });

    // Populate the result vector
    result.assign(start_it, end_it);

    return result;
}