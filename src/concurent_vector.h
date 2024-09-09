#pragma once
#include <vector>
#include <mutex>
#include <format>
#include <assert.h>

template<class T>
class ConcurentVector {
private:
    std::vector<T> data;
    mutable std::mutex lock;

    void checkIndex(int index) const {
        assert((index >= 0 && index < data.size()));
    }

public:
    ConcurentVector() = default;
    ConcurentVector(int size) : data(size) {}

    void clear() {
        lock.lock();
        data.clear();
        lock.unlock();
    }

    void shrink_to_fit(){
        lock.lock();
        data.shrink_to_fit();
        lock.unlock();
    }

    void push_back(const T& elem) {
        lock.lock();
        data.push_back(elem);
        lock.unlock();
    }

    void push_back(T&& elem) {
        lock.lock();
        data.push_back(std::move(elem));
        lock.unlock();
    }

    int size() const {
        lock.lock();
        const auto s = data.size();
        lock.unlock();
        return s;
    }

    T operator[](size_t index) const {
        lock.lock();
        checkIndex(index);
        const T& result = data[index];
        lock.unlock();
        return result;
    }
};