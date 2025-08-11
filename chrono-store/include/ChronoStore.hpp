#pragma once
#include <vector>
#include <map>
#include "IChronoStore.hpp"
#include "TickData.hpp"

class ChronoStore : public IChronoStore
{
public:
    void ingest(const std::vector<TickData> &ticks) override;
    std::vector<TickData> query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const override;

private:
    std::vector<TickData> data_store_;
    std::map<std::string, std::vector<TickData>> ticks_by_symbol_;
};
