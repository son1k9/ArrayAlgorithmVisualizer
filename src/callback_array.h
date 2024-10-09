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
    std::vector<short> data_;

public:
    CallbackArray(int size) : data_(size) {};
    CallbackArray(const std::vector<short>& data_) : data_{ data_ } {};
    CallbackArray(std::vector<short>&& data_) : data_{ std::move(data_) } {};

    const std::vector<short>& data() const {
        return data_;
    }

    int size() const {
        return data_.size();
    }

    void resize(size_t size) {
        data_.resize(size);
    }

    short at(size_t index) const {
        const int value = data_[index];
        if (callback && getCallback) {
            getCallback(index, value);
        }
        return value;
    }

    void set(int index, short value) {
        int oldValue = data_[index];
        data_[index] = value;
        if (callback && setCallback) {
            setCallback(index, oldValue, value);
        }
    }

    void swap(int index1, int index2) {
        std::swap(data_[index1], data_[index2]);
        if (callback && setCallback && getCallback) {
            getCallback(index1, data_[index2]);
            getCallback(index2, data_[index1]);
            setCallback(index1, data_[index2], data_[index1]);
            setCallback(index2, data_[index1], data_[index2]);
        }
    }

    void setGetCallback(GetCallback callback) {
        getCallback = callback;
    }

    void setSetCallback(SetCallback callback) {
        setCallback = callback;
    }
};