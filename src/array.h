#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <format>
#include <assert.h>

class Array {
public:
    //Parameters: index, value
    using GetCallback = std::function<void(int, int)>;
    //Parameters: index, oldValue, newValue 
    using SetCallback = std::function<void(int, int, int)>;
    //Parameters: index1, index2 
    using SwapCallback = std::function<void(int, int)>;

    bool callback = true;

private:
    GetCallback getCallback;
    SetCallback setCallback;
    SwapCallback swapCallback;
    std::vector<int> data;

    void checkIndex(int index) const {
        assert((index >= 0 && index < data.size()));
    }

public:
    Array(int size) : data(size) {};
    Array(const std::vector<int>& data) : data{ data } {};
    Array(std::vector<int>&& data) : data{ std::move(data) } {};

    int size() const {
        return data.size();
    }

    int at(size_t index) const {
        checkIndex(index);
        const int value = data[index];
        if (callback && getCallback) {
            getCallback(index, value);
        }
        return value;
    }

    void set(int index, int value) {
        checkIndex(index);
        int oldValue = data[index];
        data[index] = value;
        if (callback && setCallback) {
            setCallback(index, oldValue, value);
        }
    }

    void swap(int index1, int index2) {
        checkIndex(index1);
        checkIndex(index2);
        std::swap(data[index1], data[index2]);
        if (callback && swapCallback) {
            swapCallback(index1, index2);
        }
    }

    void setGetCallback(const GetCallback& callback) {
        getCallback = callback;
    }

    void setSetCallback(const SetCallback& callback) {
        setCallback = callback;
    }

    void setSwapCallback(const SwapCallback& callback) {
        swapCallback = callback;
    }
};