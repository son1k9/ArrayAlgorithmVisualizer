#pragma once

enum class OperationType {
    Read,
    Write,
    Swap
};

class Operation {
public:
    const OperationType type;
protected:
    Operation(OperationType type) : type{ type } {};
};

class ReadOperation : public Operation {
public:
    const int index;
    ReadOperation(int index) : Operation(OperationType::Read), index{ index } {}
};

class WriteOperation : public Operation {
public:
    const int index;
    const int oldValue;
    const int newValue;
    WriteOperation(int index, int oldValue, int newValue)
        : Operation(OperationType::Write), index{ index }, oldValue{ oldValue }, newValue{ newValue } {}
};

class SwapOperation : public Operation {
public:
    const int index1;
    const int index2;
    SwapOperation(int index1, int index2) : Operation(OperationType::Swap), index1{ index1 }, index2{ index2 } {}
};