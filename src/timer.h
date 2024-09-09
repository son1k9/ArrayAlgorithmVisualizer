#pragma once
#include <string>
#include "raylib.h"

class Timer {
private:
    double timeStart{};
public:
    Timer() {}

    //If timer is running restarts it
    void start() {
        timeStart = GetTime();
    }

    double timePassed() const {
        return GetTime() - timeStart;
    }
};