#include "RingBuffer.hpp"
#include "TickData.hpp"

#include <algorithm>
#include <chrono>  // std::chrono::milliseconds
#include <cstddef> // std::size_t
#include <span>    // std::span
#include <vector>  // std::vector
#include <utility> // std::pair

template <typename T>
RingBuffer<T>::RingBuffer(size_t capacity)
    : buffer_(capacity), capacity_(capacity), head_(0), tail_(0), size_(0) {}

template <typename T>
void RingBuffer<T>::push(const std::vector<T> &items)
{
    size_t n = items.size();
    if (n == 0)
        return;

    size_t first_copy = std::min(n, capacity_ - head_);
    std::copy(items.begin(), items.begin() + first_copy, buffer_.begin() + head_);

    if (n > first_copy)
        std::copy(items.begin() + first_copy, items.end(), buffer_.begin());

    head_ = (head_ + n) & (capacity_ - 1);
    if (size_ + n <= capacity_)
    {
        size_ += n;
    }
    else
    {
        size_ = capacity_;
        tail_ = (head_ + capacity_ - size_) & (capacity_ - 1);
    }
}

template <typename T>
typename RingBuffer<T>::RingBufferSpanRange RingBuffer<T>::get_last_n(size_t n) const
{

    n = std::min(n, size_);
    if (n == 0)
        return {{}, {}};

    size_t start_idx = (head_ + capacity_ - n) % capacity_;

    if (start_idx < head_)
    {
        return {std::span<const T>(&buffer_[start_idx], n), {}};
    }
    else
    {
        size_t first_len = capacity_ - start_idx;
        size_t second_len = n - first_len;
        return {std::span<const T>(&buffer_[start_idx], first_len),
                std::span<const T>(&buffer_[0], second_len)};
    }
}

template <typename T>
typename RingBuffer<T>::RingBufferSpanRange RingBuffer<T>::get_last_t(std::chrono::milliseconds duration) const
{
    if (size_ == 0)
        return {{}, {}};

    size_t latest_idx = (head_ + capacity_ - 1) & (capacity_ - 1);
    auto latest_ts = buffer_[latest_idx].timestamp;
    auto cutoff = latest_ts - duration.count();

    size_t n = 0;
    while (n < size_)
    {
        size_t idx = (latest_idx + capacity_ - n) & (capacity_ - 1);
        if (buffer_[idx].timestamp < cutoff)
            break;
        ++n;
    }

    return get_last_n(n);
}

template <typename T>
size_t RingBuffer<T>::size() const
{
    return size_;
}

template <typename T>
bool RingBuffer<T>::full() const
{
    return size_ == capacity_;
}

// ---------------- Explicit template instantiation ---------------- //
template class RingBuffer<TickData>;