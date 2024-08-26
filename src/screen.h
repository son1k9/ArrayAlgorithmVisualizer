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
#include "shuffle.h"
#include "visual_array.h"

inline constexpr int maxSize = 1024;

class MainScreen {
private:
    int size = 10;
    int swapOperationDelay = 1;
    int readOperationDelay = 1;
    int writeOperationDelay = 1;

    std::vector<Algorithm> sortingAlgorithms;
    std::string sortingAlgorithmsString;
    std::vector<Algorithm> shuffleAlgorithms;
    std::string shuffleAlgorithmsString;
    VisualArray displayArray;

    int chosenAlgorithm = 0;
    bool algorithmEdit = false;
    int chosenAlgorithmType = 0;
    std::string algorithmTypeString;
    bool algorithmTypeEdit = false;
    bool sizeEdit = false;
    bool readDelayEdit = false;
    bool writeDelayEdit = false;
    bool swapDelayEdit = false;
    bool fullscreenMode = false;
    const char* algorithmsStringPtr{};

    bool isAlgorithmActive() {
        return displayArray.getState() == VisualArray::State::Running || displayArray.getState() == VisualArray::State::Paused;
    }

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
            const Rectangle labelAlgorithmType{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle comboBoxAlgType{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle labelAlgorithm{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle comboBoxAlg{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle labelSize{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle spinnerSize{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle checkboxShuffle{ controlX, increaseY(), controlHeight, controlHeight };
            const Rectangle labelReadDelay{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle spinnerReadDelay{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle labelWriteDelay{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle spinnerWriteDelay{ controlX, increaseY(),  controlWidth, controlHeight };
            const Rectangle labelSwapDelay{ controlX, increaseY(), controlWidth, controlHeight };
            const Rectangle spinnerSwapDelay{ controlX, increaseY(), controlWidth, controlHeight };
            GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
            if (algorithmEdit || algorithmTypeEdit || isAlgorithmActive()) {
                GuiLock();
            }
            GuiLabel(labelSwapDelay, "Swap delay");
            if (GuiSpinner(spinnerSwapDelay, NULL, &swapOperationDelay, 0, 10, swapDelayEdit)) {
                swapDelayEdit = !swapDelayEdit;
            }
            GuiLabel(labelWriteDelay, "Write delay");
            if (GuiSpinner(spinnerWriteDelay, NULL, &writeOperationDelay, 0, 10, writeDelayEdit)) {
                writeDelayEdit = !writeDelayEdit;
            }
            GuiLabel(labelReadDelay, "Read delay");
            if (GuiSpinner(spinnerReadDelay, NULL, &readOperationDelay, 0, 10, readDelayEdit)) {
                readDelayEdit = !readDelayEdit;
            }
            GuiLabel(labelSize, "Array size");
            if (GuiSpinner(spinnerSize, NULL, &size, 2, maxSize, sizeEdit && !isAlgorithmActive())) {
                sizeEdit = !sizeEdit;
            }
            GuiLabel(labelAlgorithm, "Algorithm");
            if (GuiDropdownBox(comboBoxAlg, algorithmsStringPtr, &chosenAlgorithm, algorithmEdit && !isAlgorithmActive())) {
                algorithmEdit = !algorithmEdit;
            }
            GuiLabel(labelAlgorithmType, "Algorithm type");
            if (GuiDropdownBox(comboBoxAlgType, algorithmTypeString.c_str(), &chosenAlgorithmType, algorithmTypeEdit && !isAlgorithmActive())) {
                algorithmTypeEdit = !algorithmTypeEdit;
                if (!algorithmTypeEdit) {
                    switch (chosenAlgorithmType)
                    {
                    case static_cast<int>(AlgorithmType::Sort):
                        algorithmsStringPtr = sortingAlgorithmsString.c_str();
                        break;

                    case static_cast<int>(AlgorithmType::Shuffle):
                        algorithmsStringPtr = shuffleAlgorithmsString.c_str();
                        break;
                    }
                    chosenAlgorithm = 0;
                }
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

    bool isGuiEditMode() const {
        return algorithmEdit || sizeEdit;
    }

    void init() {
        sortingAlgorithms.push_back(Algorithm{ quickSort, "Quick sort" });
        sortingAlgorithms.push_back(Algorithm{ insertionSort, "Insertion sort" });
        shuffleAlgorithms.push_back(Algorithm{ standartShuffle, "Standart shuffle" });
    }

    void initGui() {
        for (const auto& alg : sortingAlgorithms) {
            sortingAlgorithmsString += alg.name + ";";
        }
        if (sortingAlgorithmsString.length() > 0) {
            sortingAlgorithmsString.pop_back();
        }

        for (const auto& alg : shuffleAlgorithms) {
            shuffleAlgorithmsString += alg.name + ";";
        }
        if (shuffleAlgorithmsString.length() > 0) {
            shuffleAlgorithmsString.pop_back();
        }

        algorithmTypeString = "Sort;Shuffle";
        algorithmsStringPtr = sortingAlgorithmsString.c_str();
    }

public:
    MainScreen() : displayArray{ size } {
        init();
        initGui();
    }

    void processInput() {
        if (isGuiEditMode()) {
            return;
        }
        auto currentState = displayArray.getState();

        if (IsKeyPressed(KEY_F)) {
            fullscreenMode = !fullscreenMode;
        }

        if (IsKeyPressed(KEY_F11)) {
            Rayutils::toggleFullScreen();
        }

        if (IsKeyPressed(KEY_LEFT)) {
            displayArray.prevOperation();
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            displayArray.nextOperation();
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            switch (displayArray.getState())
            {
            case VisualArray::State::Running:
                displayArray.pause();
                break;

            case VisualArray::State::Idle:
                switch (chosenAlgorithmType)
                {
                case static_cast<int>(AlgorithmType::Sort):
                    displayArray.start(sortingAlgorithms[chosenAlgorithm]);
                    break;

                case static_cast<int>(AlgorithmType::Shuffle):
                    displayArray.start(shuffleAlgorithms[chosenAlgorithm]);
                    break;
                }
                break;

            case VisualArray::State::Paused:
                displayArray.cont();
                break;

            default:
                break;
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE)) {
            displayArray.stop();
        }
    }

    void update() {
        if ((size > 1) && (size <= maxSize) && (size != displayArray.size())) {
            displayArray.resize(size);
        }
        displayArray.update();
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