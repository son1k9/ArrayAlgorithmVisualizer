#pragma once
#include <vector>
#include <string_view>
#include <iostream>
#include <functional>
#include <thread>
#include <format>
#include "raylib.h"
#include "concurent_vector.h"
#include "callback_array.h"
#include "operation.h"
#include "timer.h"

class VisualArray {
public:
    enum class State {
        Running,
        Paused,
        Idle
    };

private:
    ConcurentVector<Operation> operations;
    int _currentReadIndex = -1;
    int nextOperationIndex = 0;
    std::vector<int> displayArray;
    CallbackArray sortArray;
    State state = State::Idle;
    Timer timer;

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

        case OperationType::SortEnd:
            processSortEnd(operation);
            break;
        }
    }

    bool tryProcessOperation(const Operation& operation) {
        switch (operation.type)
        {
        case OperationType::Read:
            if (timer.timePassed() < readOperationDelay) {
                return false;
            }
            _currentReadIndex = -1;
            processRead(operation, false);
            break;

        case OperationType::Write:
            if (timer.timePassed() < writeOperationDelay) {
                return false;
            }
            _currentReadIndex = -1;
            processWrite(operation, false);
            break;

        case OperationType::Swap:
            if (timer.timePassed() < swapOperationDelay) {
                return false;
            }
            _currentReadIndex = -1;
            processSwap(operation);
            break;

        case OperationType::SortEnd:
            _currentReadIndex = -1;
            processSortEnd(operation);
            break;
        }
        timer.start();
        return true;
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

    void processSortEnd(const Operation& operation) {
        state = State::Idle;
    }

    void changedStateDebug(State from, State to) {
        std::cout << std::format("State changed from {} to {}\n", stateToString(from), stateToString(to));
    }
public:
    VisualArray(int size) : sortArray(size), displayArray(size) {
        sortArray.setGetCallback([this](int index, int value) {
            this->getCallback(index, value);
            });
        sortArray.setSetCallback([this](int index, int oldValue, int newValue) {
            this->setCallback(index, oldValue, newValue);
            });
        sortArray.setSwapCallback([this](int index1, int index2) {
            this->swapCallback(index1, index2);
            });
        fillArray(size);
    }

    VisualArray(const VisualArray&) = delete;
    VisualArray(VisualArray&&) = delete;
    VisualArray& operator=(const VisualArray&) = delete;
    VisualArray& operator=(VisualArray&&) = delete;

    int operator[](size_t index) {
        return displayArray[index];
    }

    int currentReadIndex(){
        return _currentReadIndex;
    }

    float swapOperationDelay = 0.1f;
    float readOperationDelay = 0.1f;
    float writeOperationDelay = 0.1f;

    void swapIsSingleOpearion(bool value) {
        if (state != State::Idle) {
            return;
        }
        sortArray.swapIsSingleOperation = value;
    }

    bool swapIsSingleOperation() const {
        return sortArray.swapIsSingleOperation;
    }

    size_t size() const {
        return displayArray.size();
    }

    static std::string_view stateToString(VisualArray::State state) {
        switch (state)
        {
        case VisualArray::State::Running:
            return "Running";
            break;
        case VisualArray::State::Paused:
            return "Paused";
            break;
        case VisualArray::State::Idle:
            return "Idle";
            break;
        default:
            break;
        }
    }

    State getState() const {
        return state;
    }

    bool prevOperation() {
        if (state != State::Paused) {
            return false;
        }
        if (nextOperationIndex < operations.size()) {
            if (nextOperationIndex > 1) {
                processOperation(operations[--nextOperationIndex], true);
                if (nextOperationIndex > 1) {
                    const auto& op = operations[nextOperationIndex - 1];
                    if (op.type == OperationType::Read) {
                        processRead(op, false);
                    }
                }
            }
        }
        return true;
    }

    bool nextOperation() {
        if (state != State::Paused) {
            return false;
        }
        if (nextOperationIndex < operations.size()) {
            processOperation(operations[nextOperationIndex++]);
        }
        return true;
    }

    bool start(const Algorithm& algorithm) {
        if (state != State::Idle) {
            return false;
        }
        operations.clear();
        _currentReadIndex = -1;
        nextOperationIndex = 0;
        timer.start();
        const State oldState = state;
        std::thread th([this](Algorithm algorithm) {
            algorithm.use(this->sortArray);
            this->operations.push_back(createSortEndOperation());
            }, algorithm);
        th.detach();
        state = State::Running;
        changedStateDebug(oldState, state);
        return true;
    }

    bool pause() {
        if (state != State::Running) {
            return false;
        }
        state = State::Paused;
        return true;
    }

    bool stop() {
        if (state != State::Running && state != State::Paused) {
            return false;
        }
        state = State::Idle;
        //Kill sorting thread (how?)
        return true;
    }

    bool cont() {
        if (state != State::Paused) {
            return false;
        }
        state = State::Running;
        return true;
    }

    bool resize(int size) {
        if (state != State::Idle) {
            return false;
        }
        displayArray.resize(size);
        sortArray.resize(size);
        fillArray(size);
        return true;
    }

    void update() {
        if (state != State::Running) {
            return;
        }
        timer.update();
        if (nextOperationIndex < operations.size()) {
            if (tryProcessOperation(operations[nextOperationIndex])) {
                nextOperationIndex++;
            }
        }
    }
};
