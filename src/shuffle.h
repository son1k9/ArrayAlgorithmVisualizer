#pragma once
#include "callback_array.h"
#include "random.h"

void standartShuffle(CallbackArray& array) {
    for(int i = 0; i < array.size(); i++) {
        array.swap(i, Random::geti(i, array.size() - 1));
    }
}