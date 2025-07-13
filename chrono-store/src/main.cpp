#include <iostream>
#include <vector>
#include <string>

#include "NaiveChronoStore.hpp"
#include "TickData.hpp"

// Generate synthetic tick data for benchmarking
std::vector<TickData> generate_ticks(const std::string &symbol, uint64_t start_ts, int count)
{
    std::vector<TickData> ticks;
    ticks.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        ticks.push_back(TickData{
            symbol,
            static_cast<int64_t>(start_ts + i),
            100.0 + (i % 50) * 0.1,
            100 + (i % 20)});
    }

    return ticks;
}

// Method to benchmark tick data ingestion
void benchmark_ingest(IChronoStore &store, const std::vector<TickData> &ticks)
{
    auto start = std::chrono::high_resolution_clock::now();
    store.ingest(ticks);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Ingested " << ticks.size() << " ticks in " << duration << " ms\n";
}

// Method to benchmark tick data queries
void benchmark_query(const IChronoStore &store, uint64_t start_ts, uint64_t end_ts, const std::string &symbol)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = store.query(start_ts, end_ts, symbol);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Queried [" << symbol << "] from " << start_ts << " to " << end_ts
              << " -> " << result.size() << " results in " << duration << " µs\n";
}

int main()
{
    std::cout << "Benchmarking NaiveChronoStore...\n";
    IChronoStore *store = new NaiveChronoStore();

    std::string symbol = "AAPL";
    uint64_t start_ts = 1609459200;
    int tick_count = 100'000'000;

    auto ticks = generate_ticks(symbol, start_ts, tick_count);

    benchmark_ingest(*store, ticks);
    benchmark_query(*store, start_ts + 1000, start_ts + 5000, "AAPL");
    benchmark_query(*store, start_ts + 10'000, start_ts + 20'000, "AAPL");
    benchmark_query(*store, start_ts + 0, start_ts + tick_count - 1, "AAPL");

    delete store;

    return 0;
}