#include <atomic>
#include <vector>
#include "shuffle.h"
#include "callback_array.h"
#include "sort.h"

static int partionMid(CallbackArray& array, int l, int r) {
    int pivot = array.at(l + (r - l) / 2);

    int i = l, j = r;

    while (true)
    {
        while (array.at(i) < pivot) {
            i++;
        }

        while (array.at(j) > pivot) {
            j--;
        }

        if (i >= j) {
            return j;
        }

        array.swap(i++, j--);
    }
}

static void quickSort_(CallbackArray& array, int l, int r, std::atomic<bool>& stop) {
    if (stop) {
        return;
    }

    if (l >= r) {
        return;
    }

    const int pivot = partionMid(array, l, r);
    quickSort_(array, l, pivot, stop);
    quickSort_(array, pivot + 1, r, stop);
}

void quickSort(CallbackArray& array, std::atomic<bool>& stop) {
    quickSort_(array, 0, array.size() - 1, stop);
}

void insertionSort(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 1; i < array.size(); i++) {
        if (stop) {
            return;
        }
        int k = array.at(i);
        int j = i - 1;
        while (j >= 0 && k < array.at(j)) {
            array.set(j + 1, array.at(j));
            j--;
        }
        array.set(j + 1, k);
    }
}

static int left(CallbackArray& array, int i) {
    return i * 2 + 1;
}

static int right(CallbackArray& array, int i) {
    return i * 2 + 2;
}

static void heapifyDown(CallbackArray& array, int i, int lenght) {
    int curr = i;
    while (curr < lenght) {
        int l = left(array, curr);
        int r = right(array, curr);
        int largest = curr;
        if (l < lenght && array.at(l) > array.at(largest)) {
            largest = l;
        }
        if (r < lenght && array.at(r) > array.at(largest)) {
            largest = r;
        }
        if (largest != curr) {
            array.swap(largest, curr);
            curr = largest;
        }
        else {
            break;
        }
    }
}

static void buildMaxHeap(CallbackArray& array, std::atomic<bool>& stop) {
    const int lenght = array.size();
    for (int i = array.size() / 2; i >= 0; i--) {
        if (stop) {
            return;
        }
        heapifyDown(array, i, lenght);
    }
}

static void extractMax(CallbackArray& array, int& lenght) {
    array.swap(0, lenght - 1);
    lenght--;
    heapifyDown(array, 0, lenght);
}

void heapSort(CallbackArray& array, std::atomic<bool>& stop) {
    buildMaxHeap(array, stop);
    int lenght = array.size();
    for (int i = 0; i < array.size(); i++) {
        if (stop) {
            return;
        }
        extractMax(array, lenght);
    }
}

void selectionSort(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 0; i < array.size() - 1; i++) {
        if (stop) {
            return;
        }

        int min = i;
        for (int j = i + 1; j < array.size(); j++) {
            if (array.at(j) < array.at(i)) {
                min = j;
            }
        }
        if (min != i) {
            array.swap(min, i);
        }
    }
}

void doubleSelectionSort(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 0; i < array.size() - 1 - i; i++) {
        if (stop) {
            return;
        }
        const int j = array.size() - 1 - i;
        int min = i;
        int max = j;
        for (int k = i; k <= j; k++) {
            if (array.at(k) < array.at(min)) {
                min = k;
            }
            if (array.at(k) > array.at(max)) {
                max = k;
            }
        }

        array.swap(i, min);
        if (max == i) {
            max = min;
        }
        array.swap(j, max);
    }
}

void bubbleSort(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 0; i < array.size() - 1; i++) {
        if (stop) {
            return;
        }
        bool swapped = false;

        for (int j = 0; j < array.size() - i - 1; j++) {
            if (array.at(j) > array.at(j + 1)) {
                array.swap(j, j + 1);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }
}

void shakerSort(CallbackArray& array, std::atomic<bool>& stop) {
    for (int i = 0; i < array.size() / 2; i++) {
        if (stop) {
            return;
        }
        bool swapped = false;

        for (int j = i; j < array.size() - i - 1; j++) {
            if (array.at(j) > array.at(j + 1)) {
                array.swap(j, j + 1);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }

        for (int j = array.size() - i - 2; j > i; j--) {
            if (array.at(j) < array.at(j - 1)) {
                array.swap(j, j - 1);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

static bool isSorted(const CallbackArray& array) {
    for (int i = 0; i < array.size() - 1; i++) {
        if (array.at(i) > array.at(i + 1)) {
            return false;
        }
    }
    return true;
}

void bogoSort(CallbackArray& array, std::atomic<bool>& stop) {
    while (!isSorted(array)) {
        if (stop) {
            return;
        }
        standartShuffle(array, stop);
    }
}

void miracleSort(CallbackArray& array, std::atomic<bool>& stop) {
    bool sorted = false;
    while (!sorted && !stop) {
        sorted = isSorted(array);
    }
}

static void merge(CallbackArray& array, int l, int mid, int r, CallbackArray& copy) {
    int i, j, k;
    for (i = l, j = mid + 1, k = l; i <= mid && j <= r; k++)
    {
        if (copy.at(i) <= copy.at(j)) {
            array.set(k, copy.at(i++));
        }
        else {
            array.set(k, copy.at(j++));
        }
    }

    while (i <= mid) {
        array.set(k++, copy.at(i++));
    }

    while (j <= r) {
        array.set(k++, copy.at(j++));
    }
}

static void mergeSort_(CallbackArray& array, int l, int r, CallbackArray& copy, std::atomic<bool>& stop) {
    if (stop) {
        return;
    }

    if (l >= r) {
        return;
    }

    int mid = l + (r - l) / 2;
    mergeSort_(copy, l, mid, array, stop);
    mergeSort_(copy, mid + 1, r, array, stop);
    merge(array, l, mid, r, copy);
}

void mergeSort(CallbackArray& array, std::atomic<bool>& stop) {
    CallbackArray copy(array.data());
    mergeSort_(array, 0, array.size() - 1, copy, stop);
}