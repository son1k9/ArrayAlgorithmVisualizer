#pragma once
#include <vector>
#include "raylib.h"
#include "raygui.h"
#include "rayutils.h"
#include "algorithm.h"
#include "sort.h" 
#include "shuffle.h"
#include "visual_array.h"


class MainScreen {
private:
    const int maxSize = GetMonitorHeight(GetCurrentMonitor()) - 40;
    enum class State {
        Idle,
        Running
    };

    State state = State::Idle;

    int size{};
    int readDelay{};
    int writeDelay{};

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
    bool fullscreenMode = false;
    bool drawHelp = true;
    const char* algorithmsStringPtr{};

    bool needToStop = false;

    bool isAlgorithmActive() const {
        return state == State::Running;
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
            settingsGroupBox.height = increaseY();
            GuiGroupBox(settingsGroupBox, "Settings");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
            if (algorithmEdit || algorithmTypeEdit) {
                GuiLock();
            }
            if (state == State::Running) {
                GuiDisable();
            }
            GuiLabel(labelWriteDelay, "Write delay");
            if (GuiSpinner(spinnerWriteDelay, NULL, &writeDelay, 0, 10, writeDelayEdit)) {
                writeDelayEdit = !writeDelayEdit;
            }
            GuiLabel(labelReadDelay, "Read delay");
            if (GuiSpinner(spinnerReadDelay, NULL, &readDelay, 0, 10, readDelayEdit)) {
                readDelayEdit = !readDelayEdit;
            }
            if (isAlgorithmActive()) {
                GuiDisable();
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
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
            GuiUnlock();
            GuiEnable();
            };

        drawSettings();
        constexpr int startButtonHeight = 50;
        Rectangle button{ margin, leftPanel.height - margin - startButtonHeight, leftPanel.width - 2 * margin, startButtonHeight };
        switch (state)
        {
        case State::Idle:
            if (GuiButton(button, "Start")) {
                start();
            }
            break;

        case State::Running:
            if (GuiButton(button, "Stop")) {
                stop();
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
        const Rectangle labelSpace{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelStop{ controlX, increaseY(), controlWidth, controlHeight };
        const Rectangle labelToggleHelp{ controlX, increaseY(), controlWidth, controlHeight };
        GuiLabel(labelToggleFullscreen, "F11 - Toggle fullscreen");
        GuiLabel(labelFullscreenMode, "F - Toggle fullscreen mode");
        GuiLabel(labelSpace, "Space - Start/Pause");
        GuiLabel(labelToggleHelp, "F1 - Toggle this panel");
        GuiLabel(labelStop, "Backspace - Stop");
        helpGroupBox.height = increaseY();
        GuiGroupBox(helpGroupBox, "Help");
    }

    void drawArray(const Rectangle& rect, const VisualArray& displayArray) {
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
        return algorithmEdit || sizeEdit || readDelayEdit || writeDelayEdit;
    }

    void init() {
        sortingAlgorithms.push_back(Algorithm{ quickSort, "Quick sort" });
        sortingAlgorithms.push_back(Algorithm{ mergeSort, "Merge sort" });
        sortingAlgorithms.push_back(Algorithm{ insertionSort, "Insertion sort" });
        sortingAlgorithms.push_back(Algorithm{ heapSort, "Heap sort" });
        sortingAlgorithms.push_back(Algorithm{ selectionSort, "Selection sort" });
        sortingAlgorithms.push_back(Algorithm{ doubleSelectionSort, "Double selection sort" });
        sortingAlgorithms.push_back(Algorithm{ bubbleSort, "Bubble sort" });
        sortingAlgorithms.push_back(Algorithm{ shakerSort, "Shaker sort" });
        sortingAlgorithms.push_back(Algorithm{ bogoSort, "Bogo sort" });
        sortingAlgorithms.push_back(Algorithm{ miracleSort, "Miracle Sort" });

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
        needToStop = false;
        state = State::Running;
        switch (chosenAlgorithmType)
        {
        case static_cast<int>(AlgorithmType::Sort):
            return displayArray.start(sortingAlgorithms[chosenAlgorithm]);

        case static_cast<int>(AlgorithmType::Shuffle):
            return displayArray.start(shuffleAlgorithms[chosenAlgorithm]);
        }
    }

    void stop() {
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

        else if (IsKeyPressed(KEY_SPACE)) {
            if (state == State::Idle) {
                start();
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE)) {
            stop();
        }
    }
public:
    MainScreen(int size = 10, int readDelay = 1, int writeDelay = 1)
        : size{ size }, readDelay{ readDelay }, writeDelay{ writeDelay }, displayArray{ size, readDelay, writeDelay } {
        init();
        initGui();
    }

    void update() {
        processInput();

        if (state == State::Idle) {
            if ((size > 1) && (size <= maxSize) && (size != displayArray.size())) {
                displayArray.resize(size);
            }
            if (readDelay >= 0 && readDelay <= 10) {
                displayArray.readDelay = readDelay;
            }
            if (writeDelay >= 0 && writeDelay <= 10) {
                displayArray.writeDelay = writeDelay;
            }
        }

        if (state == State::Running) {
            if (!displayArray.running()) {
                state = State::Idle;
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
        drawArray(arrayRect, displayArray);
    }
};