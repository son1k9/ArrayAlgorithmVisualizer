#pragma once
#include <memory>

enum class OperationType {
    Read,
    Write,
    Swap,
    SortStart,
    SortEnd
};

struct Operation {
    OperationType type{};
    int index{};
    //Read: 0
    //Write: oldValue
    //Swap: index2
    int value1{};
    //Read: 0
    //Write: newValue
    //Swap: 0 
    int value2{};
};

inline Operation createReadOperation(int index) {
    return Operation{ OperationType::Read, index };
}

inline Operation createWriteOperation(int index, int oldValue, int newValue) {
    return Operation{ OperationType::Write, index, oldValue, newValue };
}

inline Operation createSwapOperation(int index1, int index2) {
    return Operation{ OperationType::Swap, index1, index2 };
}

inline Operation createSortEndOperation() {
    return Operation{ OperationType::SortEnd };
}

inline Operation createSortStartOpearion() {
    return Operation{ OperationType::SortStart };
}