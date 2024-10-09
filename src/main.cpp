#include <iostream>
#include "raylib.h"
#include "raygui.h"
#include "style_dark.h"
#include "audio.h"
#include "screen.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Algorithm visualizer");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    InitAudioDevice();
    initAudio();

    GuiLoadStyleDark();
    Font font = LoadFont("fonts/TerminusTTF-Bold.ttf");
    if (font.texture.id > 0) {
        GuiSetFont(font);
    }

    MainScreen screen;
    while (!WindowShouldClose()) {
        screen.update();

        BeginDrawing();
        screen.draw();
        EndDrawing();
    }

    unloadAudio();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
