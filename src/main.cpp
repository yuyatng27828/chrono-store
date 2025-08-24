#include "CandleData.hpp"
#include "ChronoStore.hpp"
#include "CsvLoader.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string WHITE = "\033[37m";
const std::string LIGHT_BLUE = "\033[36m";
const std::string MAGENTA = "\033[35m";
const std::string YELLOW = "\033[33m";

std::string
format_timestamp_est(uint64_t ms_timestamp)
{
    std::time_t t = ms_timestamp / 1000;

    t -= 5 * 3600;

    std::tm tm_time;
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tm_time, &t); // use gmtime_s to avoid localtime
#else
    gmtime_r(&t, &tm_time);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S EST");
    return oss.str();
}

void print_candles_with_colored_arrows(const std::vector<CandleData> &candles)
{
    for (const auto &c : candles)
    {
        std::string arrow;
        std::string color;

        if (c.close > c.open)
        {
            arrow = "\u2191"; // ↑
            color = GREEN;
        }
        else if (c.close < c.open)
        {
            arrow = "\u2193"; // ↓
            color = RED;
        }
        else
        {
            arrow = "\u2192"; // →
            color = WHITE;
        }

        std::cout << LIGHT_BLUE << std::setw(5) << c.symbol << RESET
                  << " @ " << MAGENTA << format_timestamp_est(c.timestamp) << RESET
                  << " " << color << arrow << RESET
                  << " | O: " << std::fixed << std::setprecision(2) << std::setw(7) << c.open
                  << ", H: " << std::setw(7) << c.high
                  << ", L: " << std::setw(7) << c.low
                  << ", C: " << std::setw(7) << c.close
                  << " | " << YELLOW << "V: " << std::setw(6) << c.volume << RESET
                  << std::endl;
    }
}

void benchmark_ingest(ChronoStore &store, const std::vector<CandleData> &candles)
{
    auto start = std::chrono::high_resolution_clock::now();
    store.ingest(candles);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Ingested " << candles.size() << " candles for " << candles.front().symbol << " in " << duration << " ms\n";
}

void benchmark_query_last_n(ChronoStore &store, const std::string &symbol, size_t n)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = store.query_last_n(symbol, n);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    size_t total_candles = result.first.size() + result.second.size();
    std::cout << "Queried last " << total_candles << " candles for symbol " << symbol << " in " << duration << " ns, retrieved " << total_candles << " candles\n";
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
    ChronoStore *store = new ChronoStore();

    auto gs_candles = load_candles_from_csv("PATH TO CANDLE DATA CSV FILE", "GS");
    print_candles_with_colored_arrows(gs_candles);

    benchmark_ingest(*store, gs_candles);
    benchmark_query_last_n(*store, "GS", 100);
    benchmark_query_last_t(*store, "GS", std::chrono::milliseconds(86'400'000));

    delete store;
    return 0;
}