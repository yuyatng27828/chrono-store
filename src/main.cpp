#include "ChronoStore.hpp"
#include "TickData.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

std::vector<TickData> generate_random_ticks(ChronoStore &store, const std::vector<std::string> &symbols, uint64_t start_ts, uint64_t end_ts, int count)
{
    std::vector<TickData> ticks;
    ticks.reserve(count);

    std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<size_t> symbol_dist(0, symbols.size() - 1);
    std::uniform_int_distribution<uint64_t> ts_dist(start_ts, end_ts);
    std::uniform_int_distribution<int64_t> price_dist(100, 105);
    std::uniform_int_distribution<int64_t> volume_dist(100, 120);

    for (int i = 0; i < count; ++i)
    {
        ticks.push_back(TickData{
            symbols[symbol_dist(rng)],
            ts_dist(rng),
            price_dist(rng),
            volume_dist(rng)});
    }

    return ticks;
}

void benchmark_ingest(ChronoStore &store, const std::vector<TickData> &ticks)
{
    auto start = std::chrono::high_resolution_clock::now();
    store.ingest(ticks);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Ingested " << ticks.size() << " ticks in " << duration << " ms\n";
}

void benchmark_query_last_n(ChronoStore &store, const std::string &symbol, size_t n)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = store.query_last_n(symbol, n);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    size_t total_ticks = result.first.size() + result.second.size();
    std::cout << "Queried last " << total_ticks << " ticks for symbol " << symbol << " in " << duration << " µs, retrieved " << total_ticks << " ticks\n";
}

int main()
{
    std::cout << "Benchmarking ChronoStore...\n";
    ChronoStore *store = new ChronoStore();

    std::vector<std::string> symbols = {"AAPL", "GOOG", "MSFT", "TSLA"};
    uint64_t start_time = 1609459200; // 2021-01-01 00:00:00 UTC
    uint64_t end_time = 1609545600;   // 2021-01-02 00:00:00 UTC
    int tick_count = 1'000'000;

    auto ticks = generate_random_ticks(*store, symbols, start_time, end_time, tick_count);

    benchmark_ingest(*store, ticks);

    benchmark_query_last_n(*store, "AAPL", 100'000);

    delete store;

    return 0;
}