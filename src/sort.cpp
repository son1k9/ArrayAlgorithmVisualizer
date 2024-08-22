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