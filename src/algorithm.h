#pragma once
#include <atomic>
#include <functional>
#include "callback_array.h"

enum class AlgorithmType {
    Sort = 0,
    Shuffle,
    Count
};

struct Algorithm {
    using AlgorithmFunction = void(*)(CallbackArray&, std::atomic<bool>&);
    const std::string name;
    const AlgorithmFunction func;

    Algorithm(AlgorithmFunction func, const std::string& name)
        : name{ name }, func{ func } {}
    Algorithm(AlgorithmFunction func, std::string&& name)
        : name{ name }, func{ func } {}

    void use(CallbackArray& array, std::atomic<bool>& stop) const {
        func(array, stop);
    }
};