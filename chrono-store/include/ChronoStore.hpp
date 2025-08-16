#pragma once

#include "TickData.hpp"

#include <string>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

class ChronoStore
{
public:
    void ingest(const std::vector<TickData> &ticks);

    using TickIterator = std::vector<TickData>::const_iterator;
    std::pair<TickIterator, TickIterator> query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const;

private:
    std::unordered_map<std::string, std::vector<TickData>> ticks_by_symbol_;
    mutable std::unordered_map<std::string, std::mutex> symbol_mutexes_;
};
