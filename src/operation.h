#pragma once
#include <memory>

enum class OperationType {
    Read,
    Write,
    Swap
};

struct Operation {
    const OperationType type{};
    const int index{};
    struct Data {
        const int value1{};
        const int value2{};
    };

    //Read: nullptr
    //Write: value1 - oldValue, value2 - newValue
    //Swap: value1 - index1, value2 - index2
    const std::unique_ptr<Data> data = nullptr;
};

inline Operation CreateReadOperation(int index) {
   return Operation{OperationType::Read, index};
}

inline Operation CreateWriteOperation(int index, int oldValue, int newValue) {
    return Operation{OperationType::Read, index, std::make_unique<Operation::Data>(oldValue, newValue)};
}

Operation CreateSwapOperation(int index1, int index2) {
    return Operation{OperationType::Swap, index1, std::make_unique<Operation::Data>(index2)};
}