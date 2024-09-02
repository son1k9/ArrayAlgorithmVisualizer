#pragma once
#include <atomic>
#include "callback_array.h"
#include "random.h"

inline void standartShuffle(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 0; i < array.size(); i++) {
        if (stop) {
            return;
        }
        array.swap(i, Random::geti(i, array.size() - 1));
    }
}