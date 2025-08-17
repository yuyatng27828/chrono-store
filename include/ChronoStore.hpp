#pragma once

#include "TickData.hpp"
#include "RingBuffer.hpp"

#include <mutex>
#include <span>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class ChronoStore
{
public:
    explicit ChronoStore(size_t capacity_per_symbol = 300'000)
        : capacity_per_symbol_(capacity_per_symbol) {}

    void ingest(const std::vector<TickData> &ticks);

    using RingBufferSpan = std::span<const TickData>;
    using RingBufferSpanRange = std::pair<RingBufferSpan, RingBufferSpan>;

    RingBufferSpanRange query_last_n(const std::string &symbol, size_t n) const;
    RingBufferSpanRange query_last_t(const std::string &symbol, std::chrono::milliseconds duration) const;

private:
    std::unordered_map<std::string, RingBuffer<TickData>> ticks_by_symbol_;
    mutable std::unordered_map<std::string, std::mutex> symbol_mutexes_;
    size_t capacity_per_symbol_; // Default capacity for each symbol's ring buffer
};
