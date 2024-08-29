#pragma once
#include <vector>
#include <string_view>
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <format>
#include "raylib.h"
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
    using OperationsList = std::list<Operation>;
    std::shared_ptr<OperationsList> operations = std::make_shared<OperationsList>();
    OperationsList::iterator operationsIt;
    std::shared_ptr<CallbackArray> sortArray;
    int currOperationIndex = 0;
    int _currentReadIndex = -1;
    std::vector<int> displayArray;
    State state = State::Idle;
    Timer timer;

    void fillArray(int size) {
        sortArray->callback = false;
        for (int i = 0; i < size; i++) {
            sortArray->set(i, i + 1);
            displayArray[i] = i + 1;
        }
        sortArray->callback = true;
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

    void setCallbacks() {
        sortArray->setGetCallback([operations = operations](int index, int value) {
            operations->push_back(createReadOperation(index));
            });
        sortArray->setSetCallback([operations = operations](int index, int oldValue, int newValue) {
            operations->push_back(createWriteOperation(index, oldValue, newValue));
            });
        sortArray->setSwapCallback([operations = operations](int index1, int index2) {
            operations->push_back(createSwapOperation(index1, index2));
            });
    }
public:
    VisualArray(int size) : sortArray(std::make_shared<CallbackArray>(size)), displayArray(size) {
        fillArray(size);
        setCallbacks();
    }

    VisualArray(const VisualArray&) = delete;
    VisualArray(VisualArray&&) = delete;
    VisualArray& operator=(const VisualArray&) = delete;
    VisualArray& operator=(VisualArray&&) = delete;

    int operator[](size_t index) const {
        return displayArray[index];
    }

    int currentReadIndex() const {
        return _currentReadIndex;
    }

    float swapOperationDelay = 0.1f;
    float readOperationDelay = 0.1f;
    float writeOperationDelay = 0.1f;

    void swapIsSingleOpearion(bool value) {
        if (state != State::Idle) {
            return;
        }
        sortArray->swapIsSingleOperation = value;
    }

    bool swapIsSingleOperation() const {
        return sortArray->swapIsSingleOperation;
    }

    size_t size() const {
        return displayArray.size();
    }

    State getState() const {
        return state;
    }

    bool hasNextOperation() const {
        return currOperationIndex + 1 < operations->size();
    }

    bool hasPrevOperation() const {
        return currOperationIndex > 1;
    }

    bool prevOperation() {
        if (state != State::Paused && state != State::Idle) {
            return false;
        }
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

    bool nextOperation() {
        if (state != State::Paused && state != State::Idle) {
            return false;
        }
        if (!hasNextOperation()) {
            return false;
        }
        processOperation(*++operationsIt);
        currOperationIndex++;
    }

    bool start(const Algorithm& algorithm) {
        if (state != State::Idle) {
            return false;
        }
        _currentReadIndex = -1;
        operations->clear();
        operations->push_back(createSortStartOpearion());
        operationsIt = operations->begin();
        currOperationIndex = 0;
        timer.start();
        std::thread th([](std::shared_ptr<OperationsList> operations, std::shared_ptr<CallbackArray> array, Algorithm algorithm) {
            algorithm.use(*array);
            operations->push_back(createSortEndOperation());
            }, operations, sortArray, algorithm);
        th.detach();
        state = State::Running;
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
        operations = std::make_shared<OperationsList>();
        currOperationIndex = 0;
        sortArray = std::make_shared<CallbackArray>(size());
        setCallbacks();
        fillArray(size());
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
        sortArray->resize(size);
        fillArray(size);
        return true;
    }

    void update() {
        if (state != State::Running) {
            return;
        }
        timer.update();
        if (hasNextOperation()) {
            if (!tryProcessOperation(*++operationsIt)) {
                operationsIt--;
            }
            else {
                currOperationIndex++;
            }
        }
    }
};
