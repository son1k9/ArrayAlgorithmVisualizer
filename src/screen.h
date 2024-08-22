#pragma once
#include <vector>
#include <thread>
#include <string>
#include <numeric>
#include "raylib.h"
#include "raygui.h"
#include "rayutils.h"
#include "concurent_vector.h"
#include "algorithm.h"
#include "sort.h" 
#include "visual_array.h"

class MainScreen {
private:
    enum class State {
        Running,
        Paused,
        Idle
    };

    State currentState = State::Idle;
    std::vector<Algorithm> sortingAlgorithms;
    std::string sortingAlgorithmsString;
    VisualArray displayArray;

    //GUI
    int chosenAlgorithm = 0;
    bool algorithmEdit = false;
    int size = 10;
    bool sizeEdit = false;
    bool shuffle = false;
    bool fullscreenMode = false;

    void drawGui(const Rectangle& rect) {
        const Rectangle leftPanel{ rect.x, rect.y, rect.width, rect.height };
        DrawRectangleRec(leftPanel, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        constexpr int margin = 10;
        auto drawSettings = [this, &leftPanel]() {
            Rectangle settingsGroupBox{ leftPanel.x + margin, leftPanel.y + margin, leftPanel.width - 2 * margin, 0 };
            constexpr int gap = 5;
            constexpr int controlHeight = 25;
            const int controlWidth = settingsGroupBox.width - 2 * gap;
            const int controlX = settingsGroupBox.x + gap;
            int controlY = settingsGroupBox.y + 4 * gap - (gap + controlHeight);
            auto increaseY = [&controlY]() constexpr {
                controlY += gap + controlHeight;
                return controlY;
                };
            const Rectangle labelAlgorithm{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle comboBoxAl{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle labelSize{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle spinnerSize{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle checkboxShuffle{ controlX, increaseY(), controlHeight, controlHeight };
            GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
            GuiLabel(labelAlgorithm, "Algorithm");
            if (algorithmEdit) {
                GuiLock();
            }
            GuiLabel(labelSize, "Array size");
            if (GuiSpinner(spinnerSize, NULL, &size, 2, 512, sizeEdit)) {
                sizeEdit = !sizeEdit;
            }
            if (GuiDropdownBox(comboBoxAl, sortingAlgorithmsString.c_str(), &chosenAlgorithm, algorithmEdit)) {
                algorithmEdit = !algorithmEdit;
            }
            settingsGroupBox.height = increaseY();
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
            GuiGroupBox(settingsGroupBox, "Settings");
            GuiUnlock();
            };

        drawSettings();
        constexpr int startButtonHeight = 50;
        Rectangle button{ margin, leftPanel.height - margin - startButtonHeight, leftPanel.width - 2 * margin, startButtonHeight };
        GuiButton(button, "Start");
    }

    bool isGuiEditMode() {
        // return algorithmEdit || sizeEdit;
        return GuiGetState() == STATE_FOCUSED;
    }

    void init() {
        sortingAlgorithms.push_back(Algorithm{ quickSort, "Quick sort" });
        sortingAlgorithms.push_back(Algorithm{ insertionSort, "Insertion sort" });
    }

    void initGui() {
        for (const auto& alg : sortingAlgorithms) {
            sortingAlgorithmsString += alg.name + ";";
        }
        if (sortingAlgorithmsString.length() > 0) {
            sortingAlgorithmsString.pop_back();
        }
    }

public:
    MainScreen() {
        init();
        initGui();
    }

    void processInput() {
        if (isGuiEditMode()) {
            return;
        }

        if (IsKeyPressed(KEY_F)) {
            fullscreenMode = !fullscreenMode;
        }

        if (IsKeyPressed(KEY_F11)) {
            Rayutils::toggleFullScreen();
        }

        if (IsKeyPressed(KEY_LEFT)) {
            if (currentState == State::Paused) {
                displayArray.prevOperation();
            }
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            if (currentState == State::Paused) {
                displayArray.nextOperation();
            }
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            if (currentState == State::Running) {
                displayArray.pause();
            }
            else if (currentState == State::Paused) {
                displayArray.cont();
            }
            else if (currentState == State::Idle) {
                displayArray.start();
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE)) {
            if (currentState == State::Running || currentState == State::Paused) {
                displayArray.stop();
            }
        }
    }

    void update() {
        if (currentState == State::Paused || currentState == State::Idle) {
            return;
        }
    }

    void draw() {
        ClearBackground(BLACK);
        const int height = Rayutils::GetDisplayHeight();
        const int width = Rayutils::GetDisplayWidth();
        const Rectangle uiRect{ 0, 0, 250, height };
        Rectangle arrayRect{ 0, 0, width, height };
        if (!fullscreenMode) {
            drawGui(uiRect);
            arrayRect = { uiRect.width, 0, width - uiRect.width, static_cast<float>(height) };
        }
        displayArray.draw(arrayRect);
    }
};