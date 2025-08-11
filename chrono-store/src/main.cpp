#include <iostream>
#include <random>
#include <vector>
#include <string>

#include "ChronoStore.hpp"
#include "TickData.hpp"

std::vector<TickData> generate_random_ticks(const std::vector<std::string> &symbols, uint64_t start_ts, uint64_t end_ts, int count)
{
    std::vector<TickData> ticks;
    ticks.reserve(count);

    std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<size_t> symbol_dist(0, symbols.size() - 1);
    std::uniform_int_distribution<uint64_t> ts_dist(start_ts, end_ts);
    std::uniform_real_distribution<double> price_dist(100.0, 105.0);
    std::uniform_int_distribution<int> volume_dist(100, 120);

    for (int i = 0; i < count; ++i)
    {
        ticks.push_back(TickData{
            symbols[symbol_dist(rng)],
            static_cast<int64_t>(ts_dist(rng)),
            price_dist(rng),
            volume_dist(rng)});
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
    std::cout << "Benchmarking ChronoStore...\n";
    IChronoStore *store = new ChronoStore();

    std::vector<std::string> symbols = {"AAPL", "GOOG", "MSFT", "TSLA"};
    uint64_t start_time = 1609459200; // 2021-01-01 00:00:00 UTC
    uint64_t end_time = 1609545600;   // 2021-01-02 00:00:00 UTC
    int tick_count = 10'000'000;

    auto ticks = generate_random_ticks(symbols, start_time, end_time, tick_count);

    benchmark_ingest(*store, ticks);

    for (const auto &sym : symbols)
    {
        benchmark_query(*store, start_time, end_time, sym);
    }

    delete store;

    return 0;
}