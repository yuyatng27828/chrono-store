import csv
from datetime import datetime, timezone
import os
from pathlib import Path
from polygon import RESTClient

API_KEY = os.getenv("POLYGON_API_KEY")
SYMBOLS = ["AAPL", "GOOG", "MSFT", "TSLA", "GS"]
DATE = "2025-08-22"
OUTPUT_DIR = Path(os.getenv("POLYGON_DATA_DIR"))
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)


def fetch_candles(client, symbol: str, date: str):
    start_dt = datetime.strptime(date, "%Y-%m-%d").replace(tzinfo=timezone.utc)
    end_dt = start_dt.replace(hour=23, minute=59, second=59)

    start_ts = int(start_dt.timestamp())
    end_ts = int(end_dt.timestamp())

    candles = client.get_aggs(symbol, 1, "minute", start_ts, end_ts)
    print(f"Fetched {len(candles)} candles.")

    return candles or []


def save_to_csv(symbol: str, candles: list, date: str):
    filename = OUTPUT_DIR / f"{symbol}_{date}.csv"
    with open(filename, mode="w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["symbol", "timestamp", "open", "high", "low", "close", "volume"])
        for c in candles:
            writer.writerow([symbol, c.timestamp, c.open, c.high, c.low, c.close, c.volume])
    print(f"Saved to {filename}.")


def main():
    client = RESTClient(API_KEY)
    for symbol in SYMBOLS:
        print(f"Fetching candles for {symbol} on {DATE}...")
        candles = fetch_candles(client, symbol, DATE)
        print(f"Saving candles...")
        save_to_csv(symbol, candles, DATE)

if __name__ == "__main__":
    main()