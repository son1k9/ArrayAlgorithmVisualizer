#pragma once
#include <vector>
#include <thread>
#include "concurent_vector.h"
#include "algorithm.h" 

class MainScreen {
private:
    enum class State {
        Running,
        Paused,
        Idle
    };

    State currentState = State::Idle;
    std::vector<int> displayArray;
    ConcurentVector<int>* operaions;
    std::vector<Algorithm> sortingAlgorithms;
    std::vector<Algorithm> shuffleAlgorithms;
    std::thread workerThread;

    //GUI
    int chosenAlgorithm = 0;
    bool algorithmEdit = false;
    int size = 10;
    bool sizeEdit = false;
    bool shuffle = false;
    bool fullscreenMode = false;

    void registerSortingAlghorithm(const Algorithm& algorithm); 
    void registerShuffleAlghorithm(const Algorithm& algorithm);
    void drawGui();
    void drawArray();
    void prevOperation();
    void nextOperation();
    void pause();
    void stop();
    void cont();
    bool isGuiEditMode();
    void init();
    void initGui();
    void start();

public:
    MainScreen();
    void processInput();
    void update();
    void draw();
};