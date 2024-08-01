#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;

// Merge function
void merge(vector<double>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<double> L(n1), R(n2);

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

// Merge Sort function
void mergeSort(vector<double>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Function to calculate time and sort
double sortAndMeasureTime(vector<double>& arr) {
    auto start = chrono::high_resolution_clock::now();
    mergeSort(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    return duration.count();
}

int main() {
    vector<vector<double>> inputs = {
        {0, 1, 2, 3, 4, 5},
        {5, 5.5, 6, 3.723, 1.23, 8.88},
        // Add more input sets of varying sizes for experiments
    };

    cout << "Size\tT(n)\tt(n)\n";
    for (auto& input : inputs) {
        double tn = input.size() * log2(input.size());
        vector<double> temp = input; // Make a copy to sort
        double t = sortAndMeasureTime(temp);
        cout << input.size() << "\t" << tn << "\t" << t << "\n";
    }

    return 0;
}
