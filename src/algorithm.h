#pragma once
#include <functional>
#include <concepts>
#include "array.h"

template <std::invocable<Array&> T>
class Algorithm {
private:
    const std::string name;
    const T func;
public:
    Algorithm(const T& func, const std::string& name)
        : name{ name }, func{ func } {}
    void sort(Array& array) {
        func(array);
    }
};