#pragma once
#include <string>
#include <vector>
#include "CandleData.hpp"

std::vector<CandleData> load_candles_from_csv(const std::string &filepath, const std::string &symbol);