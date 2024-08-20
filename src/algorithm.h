#pragma once
#include <functional>
#include <concepts>
#include "array.h"

class Algorithm {
public:
    using AlgorithmFunction = void(*)(Array&);
private:
    const std::string name;
    const AlgorithmFunction func;
public:
    Algorithm(AlgorithmFunction func, const std::string& name)
        : name{ name }, func{ func } {}
    void use(Array& array) {
        func(array);
    }
};