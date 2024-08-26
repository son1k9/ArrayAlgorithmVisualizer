#pragma once
#include "array.h"
#include "random.h"

void standartShuffle(Array& array) {
    for(int i = 0; i < array.size(); i++) {
        array.swap(i, Random::geti(i, array.size() - 1));
    }
}