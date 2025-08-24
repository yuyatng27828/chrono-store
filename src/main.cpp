#include "CandleData.hpp"
#include "ChronoStore.hpp"
#include "CsvLoader.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

void benchmark_ingest(ChronoStore &store, const std::vector<CandleData> &candles)
{
    auto start = std::chrono::high_resolution_clock::now();
    store.ingest(candles);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Ingested " << candles.size() << " candles in " << duration << " ms\n";
}

void benchmark_query_last_n(ChronoStore &store, const std::string &symbol, size_t n)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = store.query_last_n(symbol, n);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    size_t total_candles = result.first.size() + result.second.size();
    std::cout << "Queried last " << total_candles << " candles for symbol " << symbol << " in " << duration << " µs, retrieved " << total_candles << " candles\n";
}

void benchmark_query_last_t(ChronoStore &store, const std::string &symbol, std::chrono::milliseconds duration)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = store.query_last_t(symbol, duration);
    auto end = std::chrono::high_resolution_clock::now();
    auto query_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    size_t total_candles = result.first.size() + result.second.size();
    std::cout << "Queried last " << duration.count() << " ms for symbol " << symbol << " in " << query_duration << " µs, retrieved " << total_candles << " candles\n";
}

int main()
{
    std::cout << "Benchmarking ChronoStore...\n";
    ChronoStore *store = new ChronoStore();

    auto aapl_candles = load_candles_from_csv("../data/polygon_candles/AAPL_2025-08-22.csv", "AAPL");

    benchmark_ingest(*store, aapl_candles);
    benchmark_query_last_n(*store, "AAPL", 100'000);
    benchmark_query_last_t(*store, "AAPL", std::chrono::milliseconds(86'400'000));

    delete store;
    return 0;
}