#ifndef TS_VECTOR_H
#define TS_VECTOR_H

#include <vector>
#include <mutex>
#include <initializer_list>
#include <algorithm>

namespace ts {

template <typename T>
class vector {
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    vector() = default;

    vector(const vector& other) {
        std::unique_lock<std::mutex> lock1(mutex_, std::defer_lock);
        std::unique_lock<std::mutex> lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = other.data_;
    }

    vector(vector&& other) noexcept {
        std::unique_lock<std::mutex> lock1(mutex_, std::defer_lock);
        std::unique_lock<std::mutex> lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = std::move(other.data_);
    }

    explicit vector(const std::vector<T>& vec) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = vec;
    }

    explicit vector(std::vector<T>&& vec) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = std::move(vec);
    }

    vector(std::initializer_list<T> init_list) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = init_list;
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            std::unique_lock<std::mutex> lock1(mutex_, std::defer_lock);
            std::unique_lock<std::mutex> lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            data_ = other.data_;
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            std::unique_lock<std::mutex> lock1(mutex_, std::defer_lock);
            std::unique_lock<std::mutex> lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            data_ = std::move(other.data_);
        }
        return *this;
    }

    vector& operator=(const std::vector<T>& other) {
        std::lock_guard lock(mutex_);
        data_ = other;
        return *this;
    }

    vector& operator=(std::vector<T>&& other) {
        std::lock_guard lock(mutex_);
        data_ = std::move(other);
        return *this;
    }

    ~vector() = default;

    void clear() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.clear();
    }

    void insert(const_iterator pos, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.insert(pos, value);
    }

    void insert(const_iterator pos, T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.insert(pos, std::move(value));
    }

    void insert(const_iterator pos, std::size_t count, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.insert(pos, count, value);
    }

    template <class InputIt>
    void insert(const_iterator pos, InputIt first, InputIt last) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.insert(pos, first, last);
    }

    void insert(const_iterator pos, std::initializer_list<T> ilist) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.insert(pos, ilist);
    }

    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.emplace(pos, std::forward<Args>(args)...);
    }

    iterator erase(const_iterator pos) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.erase(first, last);
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
    void emplace_back(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.emplace_back(std::forward<Args>(args)...);
    }

    void pop_back() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.pop_back();
    }

    void resize(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.resize(count);
    }

    void resize(size_t count, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.resize(count, value);
    }

    void swap(vector& other) {
        if (this == &other) return;
        std::scoped_lock<std::mutex, std::mutex> lock(mutex_, other._mutex);
        data_.swap(data_, other._data);
    }

    void swap(std::vector<T>& other) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.swap(data_, other);
    }

private:
    mutable std::mutex mutex_;
    std::vector<T> data_;
};

} // namespace ts

#endif // TS_VECTOR_H