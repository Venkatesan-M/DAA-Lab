#include <iostream>
#include <limits.h>
#include <vector>
#include <chrono> // Include the chrono library
#include <cstdlib> // For random number generation

using namespace std;

// Function to print optimal parenthesization
void printOptimalParens(vector<vector<int>>& s, int i, int j) {
    if (i == j)
        cout << "A" << i;
    else {
        cout << "(";
        printOptimalParens(s, i, s[i][j]);
        printOptimalParens(s, s[i][j] + 1, j);
        cout << ")";
    }
}

// Matrix Chain Multiplication using Dynamic Programming
int matrixChainOrder(vector<int>& p, int n) {
    vector<vector<int>> m(n, vector<int>(n, 0));
    vector<vector<int>> s(n, vector<int>(n, 0));

    for (int l = 2; l < n; l++) { // l is the chain length
        for (int i = 1; i < n - l + 1; i++) {
            int j = i + l - 1;
            m[i][j] = INT_MAX;
            for (int k = i; k < j; k++) {
                int q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (q < m[i][j]) {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }

    return m[1][n - 1];
}

int main() {
    vector<int> sizes = {1,5, 10,50, 100,500, 1000, 2000}; // Different sizes of input
    vector<long long> times;

    for (int n : sizes) {
        // Generate random matrix dimensions
        vector<int> p(n + 1);
        for (int i = 0; i <= n; ++i) {
            p[i] = rand() % 100 + 1; // Random dimensions between 1 and 100
        }

        auto start = chrono::high_resolution_clock::now();
        int minMultiplications = matrixChainOrder(p, n + 1); // n+1 as p has n+1 elements
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        times.push_back(duration.count());

        cout << "Input size: " << n << " -> Time: " << duration.count() << " microseconds" << endl;
    }

    // Output the times to a file for plotting
    freopen("matrix_chain_times.txt", "w", stdout);
    for (size_t i = 0; i < sizes.size(); ++i) {
        cout << sizes[i] << " " << times[i] << endl;
    }
    fclose(stdout);

    return 0;
}
