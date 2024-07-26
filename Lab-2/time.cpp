#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

void InsertionSort(int arr[], int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void measureExecutionTime(int n) {
    int arr[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = rand() % 100;
    }

    clock_t start = clock();
    InsertionSort(arr, n);
    clock_t end = clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "Input size: " << n << ", Time taken: " << duration << " seconds" << endl;
}

int main() {
    srand(time(0));
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000};
    for (int size : sizes) {
        measureExecutionTime(size);
    }
    return 0;
}
