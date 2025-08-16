#include "ChronoStore.hpp"

bool timestamp_cmp(const TickData &a, const TickData &b)
{
    return a.timestamp < b.timestamp;
}

void merge_ticks(std::vector<TickData> &existing_ticks, std::vector<TickData> &new_ticks)
{
    // Sort new ticks
    std::sort(new_ticks.begin(), new_ticks.end(), timestamp_cmp);

    // Create new vector for merged ticks
    std::vector<TickData> merged_ticks;
    merged_ticks.reserve(existing_ticks.size() + new_ticks.size());

    // Merge existing and new ticks
    std::merge(existing_ticks.begin(), existing_ticks.end(),
               new_ticks.begin(), new_ticks.end(),
               std::back_inserter(merged_ticks),
               timestamp_cmp);

    // Update existing ticks with merged ticks
    existing_ticks = std::move(merged_ticks);
}

void ChronoStore::ingest(const std::vector<TickData> &ticks)
{
    // Group ticks by symbol
    std::unordered_map<std::string, std::vector<TickData>> batches;
    for (const auto &tick : ticks)
    {
        batches[tick.symbol].push_back(tick);
    }

    for (auto &[symbol, batch] : batches)
    {
        auto &symbol_ticks = ticks_by_symbol_[symbol];
        merge_ticks(symbol_ticks, batch);
    }
}

std::vector<TickData> ChronoStore::query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const
{
    // Check if the symbol exists in the store
    auto it = ticks_by_symbol_.find(symbol);
    if (it == ticks_by_symbol_.end())
    {
        return {};
    }

    // Get the ticks for that symbol
    const auto &ticks = it->second;

    // Define helper lambdas for timestamp comparison
    auto lower_cmp = [](const TickData &tick, uint64_t ts)
    {
        return tick.timestamp < ts;
    };

    auto upper_cmp = [](uint64_t ts, const TickData &tick)
    {
        return ts < tick.timestamp;
    };

    // Find the range of ticks within the specified time range
    auto start_it = std::lower_bound(ticks.begin(), ticks.end(), start_time, lower_cmp);
    auto end_it = std::upper_bound(ticks.begin(), ticks.end(), end_time, upper_cmp);

    return {start_it, end_it};
}