#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <fstream>

using namespace std;

// Rabin-Karp Algorithm
int rabinKarpMatcher(const string &T, const string &P, int q) {
    int n = T.length();
    int m = P.length();
    int d = 10; // The number of characters in the input alphabet (0-9 for digits)
    int h = 1;  // The value of d^(m-1)
    int pHash = 0; // Hash value for pattern
    int tHash = 0; // Hash value for text
    int spuriousHits = 0;

    // Precompute h = d^(m-1) % q
    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    // Compute initial hash values for pattern and text
    for (int i = 0; i < m; i++) {
        pHash = (d * pHash + P[i]) % q;
        tHash = (d * tHash + T[i]) % q;
    }

    // Slide the pattern over text one by one
    for (int i = 0; i <= n - m; i++) {
        // Check if the hash values are the same
        if (pHash == tHash) {
            // Verify the actual characters match
            if (T.substr(i, m) == P) {
                cout << "Pattern found at index " << i << endl;
            } else {
                // Count spurious hits if hash matches but pattern does not
                spuriousHits++;
            }
        }

        // Calculate hash value for the next window of text
        if (i < n - m) {
            tHash = (d * (tHash - T[i] * h) + T[i + m]) % q;
            if (tHash < 0) {
                tHash = (tHash + q);
            }
        }
    }

    return spuriousHits;
}

int main() {
    string T = "3141592653589793";
    string P = "26";
    int q = 11; // Prime number for modulo

    // Measure time
    auto start = chrono::high_resolution_clock::now();
    int spuriousHits = rabinKarpMatcher(T, P, q);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Output the number of spurious hits and time taken
    cout << "Number of spurious hits: " << spuriousHits << endl;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    // Save results to file for graph plotting
    ofstream out("rabin_karp_times.txt", ios::app);
    out << T.length() << " " << elapsed.count() << "\n";
    out.close();

    return 0;
}
