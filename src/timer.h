#pragma once
#include <string>
#include "raylib.h"

class Timer {
private:
    float time{};
    bool active = false;
public:
    //If timer is running restarts it
    Timer() {}

    void start() {
        time = 0;
        active = true;
    }

    float timePassed() const {
        return time;
    }

    void update() {
        if (active) {
            time += GetFrameTime();
        }
    }
};