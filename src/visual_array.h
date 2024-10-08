#pragma once
#include <chrono>
#include <atomic>
#include <vector>
#include <thread>
#include "callback_array.h"

class VisualArray {
private:
    int currentReadIndex_ = -1;
    std::vector<short> displayArray;
    CallbackArray sortArray;
    std::atomic<bool> stop_ = false;
    bool running_ = false;

    void fillArray(int size) {
        sortArray.callback = false;
        for (int i = 0; i < size; i++) {
            sortArray.set(i, i + 1);
            displayArray[i] = i + 1;
        }
        sortArray.callback = true;
    }

    void getCallback(int index, short value) {
        currentReadIndex_ = index;
        if (readDelay) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(readDelay * 100));
        }
    }

    void setCallback(int index, short oldValue, short newValue) {
        displayArray[index] = newValue;
        if (writeDelay) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(writeDelay * 100));
        }
    }

public:
    VisualArray(int size, int readDelay, int writeDelay)
        : readDelay{ readDelay }, writeDelay{ writeDelay }, sortArray(size), displayArray(size) {
        sortArray.setGetCallback([this](int index, short value) {
            getCallback(index, value);
            });
        sortArray.setSetCallback([this](int index, short oldValue, short newValue) {
            setCallback(index, oldValue, newValue);
            });

        fillArray(size);
    }

    int readDelay{};
    int writeDelay{};

    VisualArray(const VisualArray&) = delete;
    VisualArray(VisualArray&&) = delete;
    VisualArray& operator=(const VisualArray&) = delete;
    VisualArray& operator=(VisualArray&&) = delete;

    bool running() const {
        return running_;
    }

    int operator[](size_t index) const {
        return displayArray[index];
    }

    int currentReadIndex() const {
        return currentReadIndex_;
    }

    size_t size() const {
        return displayArray.size();
    }

    bool start(const Algorithm& algorithm) {
        if (running_) {
            return false;
        }
        running_ = true;
        stop_ = false;
        currentReadIndex_ = -1;
        std::thread th([this](Algorithm algorithm) {
            algorithm.use(sortArray, stop_);
            running_ = false;
            }, algorithm);
        th.detach();
        return true;
    }

    bool stop() {
        if (!running_) {
            return false;
        }
        stop_ = true;
        return true;
    }

    bool resize(int size) {
        if (running_) {
            return false;
        }
        displayArray.resize(size);
        sortArray.resize(size);
        fillArray(size);
        return true;
    }
};
