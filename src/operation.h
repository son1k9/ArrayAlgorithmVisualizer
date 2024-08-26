#pragma once
#include <memory>

enum class OperationType {
    Read,
    Write,
    Swap,
    SortEnd
};

struct Operation {
    OperationType type{};
    int index{};
    struct Data {
        const int value1{};
        const int value2{};
    };

    //Read: nullptr
    //Write: value1 - oldValue, value2 - newValue
    //Swap: value1 - index2
    std::unique_ptr<Data> data = nullptr;
};

inline Operation createReadOperation(int index) {
    return Operation{ OperationType::Read, index, nullptr };
}

inline Operation createWriteOperation(int index, int oldValue, int newValue) {
    return Operation{ OperationType::Write, index, std::make_unique<Operation::Data>(oldValue, newValue) };
}

inline Operation createSwapOperation(int index1, int index2) {
    return Operation{ OperationType::Swap, index1, std::make_unique<Operation::Data>(index2) };
}

inline Operation createSortEndOperation() {
    return Operation{ OperationType::SortEnd };
}