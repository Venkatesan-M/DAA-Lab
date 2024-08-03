#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

// Merge function for strings
void merge(vector<string>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<string> L(n1), R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        } else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

// Merge Sort function for strings
void mergeSort(vector<string>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Function to generate random strings
vector<string> generateRandomStrings(int n) {
    vector<string> arr(n);
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < n; ++i) {
        string str;
        for (int j = 0; j < 5; ++j) {
            str += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        arr[i] = str;
    }
    return arr;
}

// Measure execution time
void measureExecutionTime(int n) {
    vector<string> arr = generateRandomStrings(n);

    auto start = chrono::high_resolution_clock::now();
    mergeSort(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;
    double tn = n * log2(n);
    cout << "Size: " << n << ", T(n): " << tn << ", t(n): " << duration.count() << " seconds" << endl;
}

int main() {
    srand(time(0));
    int sizes[] = {10, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    for (int size : sizes) {
        measureExecutionTime(size);
    }
    return 0;
}
