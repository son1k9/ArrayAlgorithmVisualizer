#pragma once
#include <vector>
#include <functional>

class CallbackArray {
public:
    //Parameters: index, value
    using GetCallback = std::function<void(int, short)>;
    //Parameters: index, oldValue, newValue 
    using SetCallback = std::function<void(int, short, short)>;

    bool callback = true;

private:
    GetCallback getCallback;
    SetCallback setCallback;
    std::vector<short> data;

public:
    CallbackArray(int size) : data(size) {};
    CallbackArray(const std::vector<short>& data) : data{ data } {};
    CallbackArray(std::vector<short>&& data) : data{ std::move(data) } {};

    int size() const {
        return data.size();
    }

    void resize(size_t size) {
        data.resize(size);
    }

    short at(size_t index) const {
        const int value = data[index];
        if (callback && getCallback) {
            getCallback(index, value);
        }
        return value;
    }

    void set(int index, short value) {
        int oldValue = data[index];
        data[index] = value;
        if (callback && setCallback) {
            setCallback(index, oldValue, value);
        }
    }

    void swap(int index1, int index2) {
        std::swap(data[index1], data[index2]);
        if (callback && setCallback && getCallback) {
            getCallback(index1, data[index2]);
            getCallback(index2, data[index1]);
            setCallback(index1, data[index2], data[index1]);
            setCallback(index2, data[index1], data[index2]);
        }
    }

    void setGetCallback(GetCallback callback) {
        getCallback = callback;
    }

    void setSetCallback(SetCallback callback) {
        setCallback = callback;
    }
};