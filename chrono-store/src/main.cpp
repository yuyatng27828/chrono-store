#include <iostream>
#include "chrono_store.hpp"
#include "tick_data.hpp"

int main()
{
    ChronoStore store;

    store.ingest(TickData{"AAPL", 1633072800, 150.0, 100});
    store.ingest(TickData{"AAPL", 1633076400, 151.0, 200});
    store.ingest(TickData{"GOOGL", 1633072800, 2800.0, 50});
    store.ingest(TickData{"GOOGL", 1633076400, 2810.0, 75});

    uint64_t start_time = 1633072800; // 2021-10-01 00:00:00 UTC
    uint64_t end_time = 1633076400;   // 2021-10-01 01:00:00 UTC
    std::string symbol = "GOOGL";

    std::vector<TickData> results = store.query(start_time, end_time, symbol);

    std::cout << "Query results for " << symbol << " from " << start_time << " to " << end_time << ":\n";
    for (const auto &tick : results)
    {
        std::cout << "Timestamp: " << tick.timestamp << ", Price: " << tick.price
                  << ", Volume: " << tick.volume << "\n";
    }

    return 0;
}