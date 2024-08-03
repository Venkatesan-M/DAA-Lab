#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;

// Merge function
void merge(int arr[], int p, int q, int r) {
    int n1 = q - p + 1;
    int n2 = r - q;
    int L[n1], M[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    int i = 0, j = 0, k = p;
    while (i < n1 && j < n2) {
        if (L[i] <= M[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = M[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = M[j];
        j++;
        k++;
    }
}

// Merge Sort function
void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Measure execution time
void measureExecutionTime(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = rand() % 100;
    }

    clock_t start = clock();
    mergeSort(arr, 0, n - 1);
    clock_t end = clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;
    double tn = n * log2(n);
    cout << "Size: " << n << ", T(n): " << tn << ", t(n): " << duration << " seconds" << endl;

    delete[] arr;
}

int main() {
    srand(time(0));
    int sizes[] = {10, 100,500, 1000, 5000,10000, 50000,100000,500000,1000000};
    for (int size : sizes) {
        measureExecutionTime(size);
    }
    return 0;
}
