#pragma once
#include <atomic>
#include "callback_array.h"

void quickSort(CallbackArray& array, std::atomic<bool>& stop);
void heapSort(CallbackArray& array, std::atomic<bool>& stop);
void insertionSort(CallbackArray& array, std::atomic<bool>& stop);
void selectionSort(CallbackArray& array, std::atomic<bool>& stop);
void doubleSelectionSort(CallbackArray& array, std::atomic<bool>& stop);
void bubbleSort(CallbackArray& array, std::atomic<bool>& stop);
void shakerSort(CallbackArray& array, std::atomic<bool>& stop);
void bogoSort(CallbackArray& array, std::atomic<bool>& stop);