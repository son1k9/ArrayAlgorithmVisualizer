#include "raylib.h"
#include "raygui.h"
#include "rayutils.h"
#include "screen.h"

void MainScreen::registerSortingAlghorithm(const Algorithm& algorithm) {
    sortingAlgorithms.push_back(algorithm);
}

void MainScreen::registerShuffleAlghorithm(const Algorithm& algorithm) {
    shuffleAlgorithms.push_back(algorithm);
}


void MainScreen::drawGui() {
    constexpr int leftPanelWidth = 250;
    Rectangle leftPanel{ 0, 0, leftPanelWidth, Rayutils::GetDisplayHeight() };
    DrawRectangleRec(leftPanel, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    auto drawSettings = [this]() {
        constexpr int margin = 10;
        Rectangle settingsGroupBox{ margin, margin, leftPanelWidth - 2 * margin};
        constexpr int gap = 5;
        constexpr int controlHeight = 25;
        const int controlWidth = settingsGroupBox.width - 2 * gap;
        const int controlX = settingsGroupBox.x + gap;
        int controlY = settingsGroupBox.y + 4 * gap - (gap + controlHeight);
        auto increaseY = [&controlY]() constexpr {
            controlY += gap + controlHeight;
            return controlY;
            };
        Rectangle labelAlgorithm{ controlX, increaseY(), controlWidth, controlHeight };
        Rectangle comboBoxAl{ controlX, increaseY(), controlWidth, controlHeight };
        Rectangle labelSize{ controlX, increaseY(), controlWidth, controlHeight };
        Rectangle spinnerSize{ controlX, increaseY(), controlWidth, controlHeight };
        Rectangle checkboxShuffle{ controlX, increaseY(), controlHeight, controlHeight };
        GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
        GuiLabel(labelAlgorithm, "Algorithm");
        if (algorithmEdit) {
            GuiLock();
        }
        GuiLabel(labelSize, "Size");
        if (GuiSpinner(spinnerSize, NULL, &size, 2, 512, sizeEdit)) {
            sizeEdit = !sizeEdit;
        }
        GuiCheckBox(checkboxShuffle, "Shuffle", &shuffle);
        if (GuiDropdownBox(comboBoxAl, "Quick;Heap;Merge", &chosenAlgorithm, algorithmEdit)) {
            algorithmEdit = !algorithmEdit;
        }
        settingsGroupBox.height = increaseY();
        GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        GuiGroupBox(settingsGroupBox, "Settings");
        GuiUnlock();
        };

    drawSettings();
}

bool MainScreen::isGuiEditMode() {
    // return algorithmEdit || sizeEdit;
    return GuiGetState() == STATE_FOCUSED;
}

void MainScreen::drawArray() {

}

void MainScreen::prevOperation() {

}

void MainScreen::nextOperation() {

}

void MainScreen::pause() {

}

void MainScreen::stop() {

}

void MainScreen::cont() {

}

void MainScreen::init() {

}

void MainScreen::initGui() {

}

void MainScreen::start() {

}

MainScreen::MainScreen() {
    init();
    initGui();
}

void MainScreen::processInput() {
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
            prevOperation();
        }
    }
    else if (IsKeyPressed(KEY_RIGHT)) {
        if (currentState == State::Paused) {
            nextOperation();
        }
    }
    else if (IsKeyPressed(KEY_SPACE)) {
        if (currentState == State::Running) {
            pause();
        }
        else if (currentState == State::Paused) {
            cont();
        }
        else if (currentState == State::Idle) {
            start();
        }
    }
    else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (currentState == State::Running || currentState == State::Paused) {
            stop();
        }
    }
}

void MainScreen::update() {
    if (currentState == State::Paused || currentState == State::Idle) {
        return;
    }
}

void MainScreen::draw() {
    ClearBackground(BLACK);
    if (!fullscreenMode) {
        drawGui();
    }
    drawArray();
}