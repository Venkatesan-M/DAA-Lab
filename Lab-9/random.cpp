#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <ctime>

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
                // Pattern found
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

// Function to generate random text of given length
string generateRandomText(int length) {
    string text;
    for (int i = 0; i < length; i++) {
        text += '0' + rand() % 10; // Random digit between 0 and 9
    }
    return text;
}

int main() {
    srand(time(0));
    ofstream out("rabin_karp_times.txt");

    // Pattern to search for
    string P = "26";
    int q = 11; // Prime number for modulo

    // Varying input sizes
    for (int length = 1000; length <= 10000; length += 1000) {
        // Generate random text
        string T = generateRandomText(length);

        // Measure time
        auto start = chrono::high_resolution_clock::now();
        int spuriousHits = rabinKarpMatcher(T, P, q);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // Output the number of spurious hits and time taken
        cout << "Text length: " << length << ", Time taken: " << elapsed.count() << " seconds, Spurious hits: " << spuriousHits << endl;
        
        // Save results to file for graph plotting
        out << length << " " << elapsed.count() << "\n";
    }

    out.close();
    return 0;
}
