#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

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

int main() {
    int n;
    cin >> n;
    
    int arr[n];
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }

    // Measure time taken by InsertionSort
    auto start = high_resolution_clock::now();
    InsertionSort(arr, n);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Sorted array: ";
    for (int j = 0; j < n; ++j) {
        cout << arr[j] << " ";
    }
    cout << endl;

    cout << "Time taken for sorting: " << duration.count() << " microseconds" << endl;

    return 0;
}
