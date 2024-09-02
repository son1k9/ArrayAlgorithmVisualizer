#pragma once
#include <atomic>
#include <vector>
#include <string_view>
#include <iostream>
#include <functional>
#include <thread>
#include <format>
#include "raylib.h"
#include "callback_array.h"
#include "operation.h"
#include "timer.h"

class VisualArray {
private:
    using OperationsList = std::list<Operation>;
    OperationsList operations;
    OperationsList::iterator operationsIt;
    int currOperationIndex = 0;
    int _currentReadIndex = -1;
    std::vector<int> displayArray;
    CallbackArray sortArray;
    std::atomic<bool> _stop = false;
    std::atomic<bool> _running = false;

    void fillArray(int size) {
        sortArray.callback = false;
        for (int i = 0; i < size; i++) {
            sortArray.set(i, i + 1);
            displayArray[i] = i + 1;
        }
        sortArray.callback = true;
    }

    void getCallback(int index, int value) {
        operations.push_back(createReadOperation(index));
    }

    void setCallback(int index, int oldValue, int newValue) {
        operations.push_back(createWriteOperation(index, oldValue, newValue));
    }

    void swapCallback(int index1, int index2) {
        operations.push_back(createSwapOperation(index1, index2));
    }

    void processOperation(const Operation& operation, bool prev = false) {
        _currentReadIndex = -1;
        switch (operation.type)
        {
        case OperationType::Read:
            processRead(operation, prev);
            break;

        case OperationType::Write:
            processWrite(operation, prev);
            break;

        case OperationType::Swap:
            processSwap(operation);
            break;
        }
    }

    void processRead(const Operation& operation, bool prev) {
        if (prev) {
            return;
        }
        _currentReadIndex = operation.index;
    }

    void processWrite(const Operation& operation, bool prev) {
        displayArray[operation.index] = prev ? operation.data->value1 : operation.data->value2;
    }

    void processSwap(const Operation& operation) {
        std::swap(displayArray[operation.index], displayArray[operation.data->value1]);
    }

public:
    VisualArray(int size) : sortArray(size), displayArray(size) {
        sortArray.setGetCallback([this](int index, int value) {
            getCallback(index, value);
            });
        sortArray.setSetCallback([this](int index, int oldValue, int newValue) {
            setCallback(index, oldValue, newValue);
            });
        sortArray.setSwapCallback([this](int index1, int index2) {
            swapCallback(index1, index2);
            });
        fillArray(size);
    }

    VisualArray(const VisualArray&) = delete;
    VisualArray(VisualArray&&) = delete;
    VisualArray& operator=(const VisualArray&) = delete;
    VisualArray& operator=(VisualArray&&) = delete;

    void sync() {
        if (_running) {
            return;
        }
        if (!hasNextOperation()) {
            return;
        }

        sortArray.callback = false;
        for (int i = 0; i < displayArray.size(); i++) {
            sortArray.set(i, displayArray[i]);
        }
        sortArray.callback = true;
    }

    bool running() const {
        return _running;
    }

    int operator[](size_t index) const {
        return displayArray[index];
    }

    int currentReadIndex() const {
        return _currentReadIndex;
    }

    void swapIsSingleOperation(bool value) {
        sortArray.swapIsSingleOperation = value;
    }

    bool swapIsSingleOperation() const {
        return sortArray.swapIsSingleOperation;
    }

    size_t size() const {
        return displayArray.size();
    }

    bool hasNextOperation() const {
        return currOperationIndex + 1 < operations.size();
    }

    bool hasPrevOperation() const {
        return currOperationIndex > 1;
    }

    bool prevOperation() {
        if (!hasPrevOperation()) {
            return false;
        }
        processOperation(*operationsIt--, true);
        currOperationIndex--;
        if (hasPrevOperation()) {
            const auto& op = *operationsIt;
            if (op.type == OperationType::Read) {
                processRead(op, false);
            }
        }
        return true;
    }

    const Operation* peek() {
        if (!hasNextOperation()) {
            return nullptr;
        }
        auto op = &*++operationsIt;
        operationsIt--;
        return op;
    }

    bool nextOperation() {
        if (!hasNextOperation()) {
            return false;
        }
        processOperation(*++operationsIt);
        currOperationIndex++;
    }

    bool start(const Algorithm& algorithm) {
        if (_running) {
            return false;
        }
        _running = true;
        _stop = false;
        _currentReadIndex = -1;
        operations.clear();
        currOperationIndex = 0;
        operations.push_back(createSortStartOpearion());
        operationsIt = operations.begin();
        std::thread th([this](Algorithm algorithm) {
            algorithm.use(sortArray, _stop);
            operations.push_back(createSortEndOperation());
            _running = false;
            }, algorithm);
        th.detach();
        return true;
    }

    bool clear() {
        if (_running) {
            return false;
        }
        operations.clear();
    }

    bool stop() {
        if (!_running) {
            return false;
        }
        _stop = true;
        return true;
    }

    bool resize(int size) {
        if (_running) {
            return false;
        }
        operations.clear();
        displayArray.resize(size);
        sortArray.resize(size);
        fillArray(size);
        return true;
    }
};
