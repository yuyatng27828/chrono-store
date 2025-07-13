#pragma once
#include <vector>
#include "IChronoStore.hpp"
#include "TickData.hpp"

class NaiveChronoStore : public IChronoStore
{
public:
    void ingest(const std::vector<TickData> &ticks) override;
    std::vector<TickData> query(uint64_t start_time, uint64_t end_time, const std::string &symbol) const override;

private:
    std::vector<TickData> data_store_;
};
