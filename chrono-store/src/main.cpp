#include <iostream>
#include <vector>
#include <string>
#include "chrono_store.hpp"
#include "tick_data.hpp"

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
            100.0 + (i % 50) * 0.1, // price varies slightly
            100 + (i % 20)          // volume varies
        });
    }

    return ticks;
}

// Benchmark ingestion performance
void benchmark_ingest(ChronoStore &store, const std::vector<TickData> &ticks)
{
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto &tick : ticks)
    {
        store.ingest(tick);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Ingested " << ticks.size() << " ticks in " << duration << " ms\n";
}

// Benchmark query performance
void benchmark_query(const ChronoStore &store, uint64_t start_ts, uint64_t end_ts, const std::string &symbol)
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
    ChronoStore store;

    std::string symbol = "AAPL";
    uint64_t start_ts = 1609459200; // 2021-01
    int tick_count = 1'000'000;     // Number of ticks to generate

    auto ticks = generate_ticks("AAPL", start_ts, tick_count);
    benchmark_ingest(store, ticks);

    benchmark_query(store, start_ts + 1000, start_ts + 5000, "AAPL");
    benchmark_query(store, start_ts + 10'000, start_ts + 20'000, "AAPL");
    benchmark_query(store, start_ts + 0, start_ts + tick_count - 1, "AAPL");

    return 0;
}