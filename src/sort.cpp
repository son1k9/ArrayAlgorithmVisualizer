#include "array.h"
#include "sort.h"

static int partionMid(Array& array, int l, int r) {
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

static void _quickSort(Array& array, int l, int r) {
    if (l >= r) {
        return;
    }

    const int pivot = partionMid(array, l, r);
    _quickSort(array, l, pivot);
    _quickSort(array, pivot + 1, r);
}

void quickSort(Array& array) {
    _quickSort(array, 0, array.size() - 1);
}

void insertionSort(Array& array) {
    for (int i = 1; i < array.size(); i++) {
        int k = array.at(i);
        int j = i - 1;
        while (j >= 0 && k < array.at(j)) {
            array.set(j + 1, array.at(j));
            j--;
        }
        array.set(j + 1, k);
    }
}

static int left(Array& array, int i) {
    return i * 2 + 1;
}

static int right(Array& array, int i) {
    return i * 2 + 2;
}

static void heapifyDown(Array& array, int i, int lenght) {
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

static void buildMaxHeap(Array& array) {
    const int lenght = array.size();
    for (int i = array.size() / 2; i >= 0; i--) {
        heapifyDown(array, i, lenght);
    }
}

void extractMax(Array& array, int& lenght) {
    array.swap(0, lenght - 1);
    lenght--;
    heapifyDown(array, 0, lenght);
}

void heapSort(Array& array) {
    buildMaxHeap(array);
    int lenght = array.size();
    for (int i = 0; i < array.size(); i++) {
        extractMax(array, lenght);
    }
}

void selectionSort(Array& array) {
    for (int i = 0; i < array.size() - 1; i++) {
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

void doubleSelectionSort(Array& array) {
    for (int i = 0; i < array.size() - 1 - i; i++) {
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

void bubbleSort(Array& array) {
    for (int i = 0; i < array.size() - 1; i++) {
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

void shakerSort(Array& array) {
    for (int i = 0; i < array.size() / 2; i++) {
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