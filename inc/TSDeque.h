#ifndef TS_DEQUE_H
#define TS_DEQUE_H

#include <deque>
#include <mutex>
#include <initializer_list>
#include <algorithm>

namespace ts {

template <typename T>
class deque {
public:
    deque() = default;

    deque(const deque& other) {
        std::lock_guard<std::mutex> lock1(mutex_, std::defer_lock);
        std::lock_guard<std::mutex> lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = other.data_;
    }

    deque(deque&& other) noexcept {
        std::lock_guard<std::mutex> lock1(mutex_, std::defer_lock);
        std::lock_guard<std::mutex> lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = std::move(other.data_);
    }

    deque(std::initializer_list<T> init) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = init;
    }

    deque& operator=(const deque& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(mutex_, std::defer_lock);
            std::lock_guard<std::mutex> lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            data_ = other.data_;
        }
        return *this;
    }

    deque& operator=(deque&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(mutex_, std::defer_lock);
            std::lock_guard<std::mutex> lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            data_ = std::move(other.data_);
        }
        return *this;
    }

    ~deque() = default;

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }

    T pop_front() {
        std::lock_guard<std::mutex> lock(mutex_);
        T value = std::move(data_.front());
        data_.pop_front();
        return value;
    }

    T pop_back() {
        std::lock_guard<std::mutex> lock(mutex_);
        T value = std::move(data_.back());
        data_.pop_back();
        return value;
    }

    void push_front(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push_front(value);
    }

    void push_front(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push_front(std::move(value));
    }

    void push_back(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push_back(value);
    }

    void push_back(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push_back(std::move(value));
    }

    template <class... Args>
    void emplace_front(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.emplace_front(std::forward<Args>(args)...);
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.emplace_back(std::forward<Args>(args)...);
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.clear();
    }

private:
    mutable std::mutex mutex_;
    std::deque<T> data_;
};

} // namespace ts

#endif // TS_DEQUE_H
