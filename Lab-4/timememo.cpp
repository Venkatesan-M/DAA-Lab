#include <iostream>
#include <vector>
#include <chrono>
#include <climits>

using namespace std;
using namespace std::chrono;

int rodhelp(int n, vector<int>& val, vector<int>& re) {
    if (n == 0) return 0;
    if (re[n] != -1) return re[n];

    int maxre = INT_MIN;
    for (int i = 0; i < n; i++) {
        maxre = max(maxre, val[i] + rodhelp(n - i - 1, val, re));
    }

    re[n] = maxre;
    return maxre;
}

int rodcut(int n, vector<int>& val) {
    vector<int> re(n + 1, -1);
    return rodhelp(n, val, re);
}

int main() {
    vector<int> lengths = {10, 100, 1000, 10000, 50000};
    vector<int> times;

    for (int n : lengths) {
        vector<int> prices(n);
        for (int i = 0; i < n; i++) prices[i] = i + 1;

        auto start = high_resolution_clock::now();
        cout << "Max Revenue for rod length " << n << ": " << rodcut(n, prices) << endl;
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);
        times.push_back(duration.count());
        cout << "Time taken: " << duration.count() << " microseconds" << endl;
    }

    return 0;
}
