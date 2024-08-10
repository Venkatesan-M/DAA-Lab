#include <iostream>
#include <climits>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

int rodcut(int n, int val[]) {
    int res[n + 1];
    res[0] = 0;
    
    for (int i = 1; i <= n; i++) {
        res[i] = INT_MIN;
        for (int j = 0; j < i; j++) {
            res[i] = max(res[i], val[j] + res[i - (j + 1)]);
        }
    }
    return res[n];
}

int main() {
    vector<int> lengths = {10, 100, 1000, 10000, 50000};
    vector<long long> times;

    for (int n : lengths) {
        // Create the price array
        int* prices = new int[n];
        for (int i = 0; i < n; i++) {
            prices[i] = i + 1;
        }

        // Start timing
        auto start = high_resolution_clock::now();

        // Compute maximum revenue
        cout << "Maximum Revenue for rod length " << n << ": " << rodcut(n, prices) << endl;

        // Stop timing
        auto stop = high_resolution_clock::now();

        // Calculate and store time taken
        auto duration = duration_cast<microseconds>(stop - start);
        times.push_back(duration.count());
        cout << "Time taken: " << duration.count() << " microseconds" << endl;

        // Clean up
        delete[] prices;
    }

    return 0;
}
