#include <iostream>
#include "raylib.h"
#include "raygui.h"
#include "style_dark.h"
#include "screen.h"

int main() {
    InitWindow(1600, 900, "Algorithm visualizer");
    SetTargetFPS(75);

    GuiLoadStyleDark();
    Font font = LoadFont("fonts/TerminusTTF-Bold.ttf");
    if (font.texture.id > 0) {
        GuiSetFont(font);
    }

    MainScreen screen;
    while (!WindowShouldClose()) {
        screen.processInput();
        screen.update();

        BeginDrawing();
        screen.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
