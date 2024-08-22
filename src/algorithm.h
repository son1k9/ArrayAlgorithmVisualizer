#pragma once
#include <functional>
#include "array.h"

struct Algorithm {
    using AlgorithmFunction = void(*)(Array&);
    const std::string name;
    const AlgorithmFunction func;

    Algorithm(AlgorithmFunction func, const std::string& name)
        : name{ name }, func{ func } {}
    void use(Array& array) const {
        func(array);
    }
};