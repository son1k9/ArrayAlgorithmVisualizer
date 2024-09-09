#pragma once
#include <vector>
#include <thread>
#include <string>
#include <numeric>
#include "raylib.h"
#include "raygui.h"
#include "rayutils.h"
#include "algorithm.h"
#include "sort.h" 
#include "shuffle.h"
#include "visual_array.h"

inline constexpr int maxSize = 1024;

class MainScreen {
private:
    enum class State {
        Idle,
        Running,
        Paused
    };

    State state = State::Idle;

    int size = 10;
    int swapOperationDelay = 1;
    int readOperationDelay = 1;
    int writeOperationDelay = 1;

    std::vector<Algorithm> sortingAlgorithms;
    std::string sortingAlgorithmsString;
    std::vector<Algorithm> shuffleAlgorithms;
    std::string shuffleAlgorithmsString;
    VisualArray displayArray;
    Timer timer;

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
    bool drawHelp = true;
    bool autoStop = true;
    const char* algorithmsStringPtr{};
    bool swapIsSingleOperation = true;

    bool needToStop = false;

    bool isAlgorithmActive() {
        return state == State::Running || state == State::Paused;
    }

    void drawGuiLeftPanel(const Rectangle& leftPanel) {
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
            const Rectangle checkboxSwap{ controlX, increaseY(), controlHeight, controlHeight };
            const Rectangle checkboxAutoStop{ controlX, increaseY(), controlHeight, controlHeight };
            GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
            if (algorithmEdit || algorithmTypeEdit) {
                GuiLock();
            }
            if (state == State::Running) {
                GuiDisable();
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
            if (isAlgorithmActive()) {
                GuiDisable();
            }
            GuiCheckBox(checkboxSwap, "Swap is single op", &swapIsSingleOperation);
            GuiCheckBox(checkboxAutoStop, "Auto stop", &autoStop);
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
            GuiEnable();
            };

        drawSettings();
        constexpr int startButtonHeight = 50;
        Rectangle stopButton{ margin, leftPanel.height - 2 * margin - 2 * startButtonHeight, leftPanel.width - 2 * margin, startButtonHeight };
        if (state == State::Idle) {
            GuiDisable();
        }
        if (GuiButton(stopButton, "Stop")) {
            stop();
        }
        GuiEnable();
        Rectangle button{ margin, leftPanel.height - margin - startButtonHeight, leftPanel.width - 2 * margin, startButtonHeight };
        switch (state)
        {
        case State::Idle:
            if (GuiButton(button, "Start")) {
                start();
            }
            break;

        case State::Running:
            if (GuiButton(button, "Pause")) {
                pause();
            }
            break;

        case State::Paused:
            if (GuiButton(button, "Continue")) {
                cont();
            }
            break;
        }
    }

    void drawGuiHelp(const Rectangle& rect) {
        DrawRectangleRec(rect, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        constexpr int margin = 10;
        Rectangle helpGroupBox{ rect.x + margin, rect.y + margin, rect.width - 2 * margin, 0 };
        constexpr int gap = 5;
        constexpr int controlHeight = 25;
        const int controlWidth = helpGroupBox.width - 2 * gap;
        const int controlX = helpGroupBox.x + gap;
        int controlY = helpGroupBox.y + 4 * gap - (gap + controlHeight);
        auto increaseY = [&controlY]() constexpr {
            controlY += gap + controlHeight;
            return controlY;
            };
        const Rectangle labelToggleFullscreen{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelFullscreenMode{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelPrev{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelNext{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelSpace{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelStop{controlX, increaseY(), controlWidth, controlHeight};
        const Rectangle labelToggleHelp{ controlX, increaseY(), controlWidth, controlHeight };
        GuiLabel(labelToggleFullscreen, "F11 - Toggle fullscreen");
        GuiLabel(labelFullscreenMode, "F - Toggle fullscreen mode");
        GuiLabel(labelPrev, "<- - Previous operation");
        GuiLabel(labelNext, "-> - Next operation");
        GuiLabel(labelSpace, "Space - Start/Pause");
        GuiLabel(labelToggleHelp, "F1 - Toggle this panel");
        GuiLabel(labelStop, "Backspace - Stop");
        helpGroupBox.height = increaseY();
        GuiGroupBox(helpGroupBox, "Help");
    }

    void drawArray(const Rectangle& rect) {
        const int padding = 20;
        const int elementWidth = (rect.width - padding * 2) / displayArray.size();
        const float elementWidthError = (rect.width - padding * 2) / displayArray.size() - elementWidth;
        const int xOffset = elementWidthError * displayArray.size() / 2;
        const int elementHeightDiff = (rect.height - padding * 2) / displayArray.size();
        const float elementHeightDiffError = (rect.height - padding * 2) / displayArray.size() - elementHeightDiff;
        const int yOffset = elementHeightDiffError * displayArray.size() / 2;
        const int arraySize = displayArray.size();
        const float colorStep = 360.f / (displayArray.size() - 1);
        for (int i = 0; i < displayArray.size(); i++) {
            const int val = displayArray[i];
            Color col = ColorFromHSV(colorStep * (val - 1), 1.0f, 1.0f);
            if (state != State::Idle && displayArray.currentReadIndex() == i) {
                col = WHITE;
            }
            const int width = elementWidth;
            const int height = elementHeightDiff * val;
            const int x = rect.x + padding + elementWidth * i + xOffset;
            const int y = rect.y + rect.height - padding - height - yOffset;
            Rectangle elementRect{ x, y, width, height };
            DrawRectangleRec(elementRect, col);
        }
    }

    bool isGuiEditMode() const {
        return algorithmEdit || sizeEdit || readDelayEdit || writeDelayEdit || swapDelayEdit;
    }

    void init() {
        sortingAlgorithms.push_back(Algorithm{ quickSort, "Quick sort" });
        sortingAlgorithms.push_back(Algorithm{ insertionSort, "Insertion sort" });
        sortingAlgorithms.push_back(Algorithm{ heapSort, "Heap sort" });
        sortingAlgorithms.push_back(Algorithm{ selectionSort, "Selection sort" });
        sortingAlgorithms.push_back(Algorithm{ doubleSelectionSort, "Double selection sort" });
        sortingAlgorithms.push_back(Algorithm{ bubbleSort, "Bubble sort" });
        sortingAlgorithms.push_back(Algorithm{ shakerSort, "Shaker sort" });
        sortingAlgorithms.push_back(Algorithm{ bogoSort, "Bogo sort" });

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

    bool start() {
        SetTargetFPS(-1);
        needToStop = false;
        state = State::Running;
        timer.start();
        switch (chosenAlgorithmType)
        {
        case static_cast<int>(AlgorithmType::Sort):
            return displayArray.start(sortingAlgorithms[chosenAlgorithm]);

        case static_cast<int>(AlgorithmType::Shuffle):
            return displayArray.start(shuffleAlgorithms[chosenAlgorithm]);
        }
    }

    void pause() {
        state = State::Paused;
    }

    void cont() {
        state = State::Running;
    }

    void stop() {
        SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
        needToStop = true;
        displayArray.stop();
    }

    void processInput() {
        if (isGuiEditMode()) {
            return;
        }

        if (IsKeyPressed(KEY_F)) {
            fullscreenMode = !fullscreenMode;
        }

        if (IsKeyPressed(KEY_F1)) {
            drawHelp = !drawHelp;
        }

        if (IsKeyPressed(KEY_F11)) {
            Rayutils::toggleFullScreen();
        }

        if (IsKeyPressed(KEY_LEFT)) {
            if (state == State::Paused) {
                displayArray.prevOperation();
            }
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            if (state == State::Paused) {
                displayArray.nextOperation();
            }
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            switch (state)
            {
            case State::Running:
                pause();
                break;

            case State::Idle:
                start();
                break;

            case State::Paused:
                cont();
                break;
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE)) {
            stop();
        }
    }
public:
    MainScreen() : displayArray{ size } {
        init();
        initGui();
    }

    void update() {
        processInput();
        if (state == State::Idle) {
            if (swapIsSingleOperation != displayArray.swapIsSingleOperation()) {
                displayArray.swapIsSingleOperation(swapIsSingleOperation);
            }

            if ((size > 1) && (size <= maxSize) && (size != displayArray.size())) {
                displayArray.resize(size);
            }
        }

        if (needToStop) {
            if (!displayArray.running()) {
                state = State::Idle;
                displayArray.sync();
                displayArray.clear();
            }
        }

        if (state == State::Running) {
            auto operation = displayArray.peek();
            if (operation) {
                double operationDelay{};
                switch (operation->type)
                {
                case OperationType::Read:
                    operationDelay = readOperationDelay / 1000.0f;
                    break;

                case OperationType::Write:
                    operationDelay = writeOperationDelay / 1000.0f;
                    break;

                case OperationType::Swap:
                    operationDelay = swapOperationDelay / 1000.0f;
                    break;

                default:
                    break;
                }
                if (timer.timePassed() > operationDelay) {
                    timer.start();
                    displayArray.nextOperation();
                }
            }
            else if (!displayArray.running()) {
                if (autoStop) {
                    stop();
                }
                else {
                    pause();
                }
            }
        }
    }

    void draw() {
        ClearBackground(BLACK);
        const int height = Rayutils::GetDisplayHeight();
        const int width = Rayutils::GetDisplayWidth();
        const Rectangle uiRect{ 0, 0, 250, height };
        const Rectangle helpRect{ width - 350, 0, 350, height };
        Rectangle arrayRect{ 0, 0, width, height };
        if (!fullscreenMode) {
            drawGuiLeftPanel(uiRect);
            arrayRect = { uiRect.width, 0, width - uiRect.width, static_cast<float>(height) };
            if (drawHelp) {
                drawGuiHelp(helpRect);
                arrayRect.width -= helpRect.width;
            }
        }
        drawArray(arrayRect);
    }
};