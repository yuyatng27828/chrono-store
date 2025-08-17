#pragma once

#include <chrono>  // std::chrono::milliseconds
#include <cstddef> // std::size_t
#include <vector>  // std::vector

template <typename T>
class RingBuffer
{
public:
    explicit RingBuffer(size_t capacity);

    void push(const T &item);
    void push(const std::vector<T> &items);

    using RingBufferIterPair = std::pair<typename std::vector<T>::const_iterator,
                                         typename std::vector<T>::const_iterator>;
    using RingBufferIterRange = std::pair<RingBufferIterPair, RingBufferIterPair>;

    RingBufferIterRange get_last_n(size_t n) const;
    RingBufferIterRange get_last_t(std::chrono::milliseconds duration) const;

    size_t size() const;
    bool full() const;

private:
    std::vector<T> buffer_;
    size_t capacity_;
    size_t head_;
    size_t tail_;
    size_t size_;
};