#include <iostream>
#include <vector>
#include <string>
#include <chrono> // Include for timing
#include <cstdlib> // For random character generation

using namespace std;

// Function to find LCS
pair<int, string> LCS(string X, string Y) {
    int m = X.size();
    int n = Y.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    // Fill dp table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i - 1] == Y[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    // Backtrack to find the LCS
    string lcs = "";
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            lcs = X[i - 1] + lcs;
            i--, j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    return {dp[m][n], lcs};
}

int main() {
    vector<int> sizes = {1, 10, 100, 1000, 10000}; // Varying input sizes
    vector<long long> times;

    for (int n : sizes) {
        string X, Y;

        // Generate random strings of length n
        for (int i = 0; i < n; ++i) {
            X += 'A' + rand() % 26; // Random uppercase letters
            Y += 'A' + rand() % 26;
        }

        auto start = chrono::high_resolution_clock::now();
        pair<int, string> result = LCS(X, Y);
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        times.push_back(duration.count());

        cout << "Input size: " << n << " -> Time: " << duration.count() << " microseconds" << endl;
    }

    // Output the times to a file for plotting
    freopen("lcs_times.txt", "w", stdout);
    for (size_t i = 0; i < sizes.size(); ++i) {
        cout << sizes[i] << " " << times[i] << endl;
    }
    fclose(stdout);

    return 0;
}
