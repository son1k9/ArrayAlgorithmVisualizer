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
    bool swapIsSingleOperation = true;

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

    void resize(size_t size) {
        data.resize(size);
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
            if (swapIsSingleOperation) {
                swapCallback(index1, index2);
            }
            else {
                getCallback(index1, data[index2]);
                getCallback(index2, data[index1]);
                setCallback(index1, data[index2], data[index1]);
                setCallback(index2, data[index1], data[index2]);
            }
        }
    }

    void setGetCallback(GetCallback callback) {
        getCallback = callback;
    }

    void setSetCallback(SetCallback callback) {
        setCallback = callback;
    }

    void setSwapCallback(SwapCallback callback) {
        swapCallback = callback;
    }
};