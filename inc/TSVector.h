#ifndef TS_VECTOR_H
#define TS_VECTOR_H

#include <vector>
#include <mutex>
#include <initializer_list>
#include <algorithm>

namespace ts {
template <typename T> class vector {
public:
    vector() = default;

    vector(const vector& other) {
        std::unique_lock lock1(mutex_, std::defer_lock);
        std::unique_lock lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = other.data_;
    }

    vector(vector&& other) noexcept {
        std::unique_lock lock1(mutex_, std::defer_lock);
        std::unique_lock lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        data_ = std::move(other.data_);
    }

    explicit vector(const std::vector<T>& vec) {
        std::lock_guard lock(mutex_);
        data_ = vec;
    }

    explicit vector(std::vector<T>&& vec) {
        std::lock_guard lock(mutex_);
        data_ = std::move(vec);
    }

    vector(std::initializer_list<T> init_list) {
        std::lock_guard lock(mutex_);
        data_ = init_list;
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            std::unique_lock lock1(mutex_, std::defer_lock);
            std::unique_lock lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            data_ = other.data_;
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            std::unique_lock lock1(mutex_, std::defer_lock);
            std::unique_lock lock2(other.mutex_, std::defer_lock);
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

    void clear() {
        std::lock_guard lock(mutex_);
        data_.clear();
    }

    void push_back(const T& value) {
        std::lock_guard lock(mutex_);
        data_.push_back(value);
    }

    void push_back(T&& value) {
        std::lock_guard lock(mutex_);
        data_.push_back(std::move(value));
    }

    template <class... Args>
    T& emplace_back(Args&&... args) {
        std::lock_guard lock(mutex_);
        return data_.emplace_back(std::forward<Args>(args)...);
    }

    void pop_back() {
        std::lock_guard lock(mutex_);
        data_.pop_back();
    }

    void resize(size_t count) {
        std::lock_guard lock(mutex_);
        data_.resize(count);
    }

    void resize(size_t count, const T& value) {
        std::lock_guard lock(mutex_);
        data_.resize(count, value);
    }

    void swap(vector& other) noexcept {
        if (this == &other) return;
        std::scoped_lock lock(mutex_, other.mutex_);
        data_.swap(other.data_);
    }

    void swap(std::vector<T>& other) {
        std::lock_guard lock(mutex_);
        data_.swap(other);
    }

    bool empty() const {
        std::lock_guard lock(mutex_);
        return data_.empty();
    }

    size_t size() const {
        std::lock_guard lock(mutex_);
        return data_.size();
    }

    template <typename Pred>
    void erase_if(Pred pred) {
        std::lock_guard lock(mutex_);
        data_.erase(
            std::remove_if(data_.begin(), data_.end(), pred),
            data_.end()
        );
    }

    std::vector<T> snapshot() const {
        std::lock_guard lock(mutex_);
        return data_;
    }

private:
    mutable std::mutex mutex_;
    std::vector<T> data_;
};

} // namespace ts

#endif // TS_VECTOR_H
