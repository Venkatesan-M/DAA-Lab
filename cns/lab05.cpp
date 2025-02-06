#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <cmath>

using namespace std;

/*****************************
 * Caesar Cipher
 *****************************/
string caesarCipher(const string &plaintext, int shift) {
    string result;
    for (char c : plaintext) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            // Normalize and apply shift modulo 26.
            result.push_back(char((c - base + shift) % 26 + base));
        } else {
            result.push_back(c);
        }
    }
    return result;
}

/*****************************
 * Vigenère Cipher
 *****************************/
string vigenereCipher(const string &plaintext, const string &key) {
    string result;
    int keyLen = key.size();
    int j = 0;
    for (char c : plaintext) {
        if (isalpha(c)) {
            if (isupper(c)) {
                int shift = tolower(key[j % keyLen]) - 'a';
                result.push_back(char(((c - 'A' + shift) % 26) + 'A'));
            } else {
                int shift = tolower(key[j % keyLen]) - 'a';
                result.push_back(char(((c - 'a' + shift) % 26) + 'a'));
            }
            j++;
        } else {
            result.push_back(c);
        }
    }
    return result;
}

/*****************************
 * Hill Cipher
 *****************************/
string hillCipher(const string &plaintext, const vector<vector<int>> &keyMatrix) {
    // Remove spaces and convert to uppercase.
    string text;
    for (char c : plaintext) {
        if (!isspace(c))
            text.push_back(toupper(c));
    }
    
    int n = keyMatrix.size();
    // Pad with 'X' if necessary.
    while (text.size() % n != 0) {
        text.push_back('X');
    }
    
    string result;
    // Process each block.
    for (size_t i = 0; i < text.size(); i += n) {
        vector<int> block(n);
        for (int j = 0; j < n; j++) {
            block[j] = text[i + j] - 'A';
        }
        // Multiply keyMatrix by block vector modulo 26.
        for (int row = 0; row < n; row++) {
            int sum = 0;
            for (int col = 0; col < n; col++) {
                sum += keyMatrix[row][col] * block[col];
            }
            result.push_back(char((sum % 26 + 26) % 26 + 'A'));
        }
    }
    return result;
}

/*****************************
 * Playfair Cipher
 *****************************/
vector<char> createPlayfairMatrix(const string &key) {
    vector<char> matrix;
    string keyStr;
    // Remove spaces, convert to lowercase and treat 'j' as 'i'.
    for (char c : key) {
        if (isalpha(c)) {
            char lowerC = tolower(c);
            if (lowerC == 'j')
                lowerC = 'i';
            keyStr.push_back(lowerC);
        }
    }
    // Append key characters (in order of appearance, no duplicates).
    for (char c : keyStr) {
        if (find(matrix.begin(), matrix.end(), c) == matrix.end())
            matrix.push_back(c);
    }
    // Append remaining letters (skipping 'j' since i/j are combined).
    string alphabet = "abcdefghiklmnopqrstuvwxyz"; // note: no 'j'
    for (char c : alphabet) {
        if (find(matrix.begin(), matrix.end(), c) == matrix.end())
            matrix.push_back(c);
    }
    return matrix;
}

string playfairCipher(const string &plaintext, const string &key) {
    vector<char> matrix = createPlayfairMatrix(key);
    // Prepare the plaintext: remove spaces, to lowercase, treat 'j' as 'i'.
    string text;
    for (char c : plaintext) {
        if (isalpha(c)) {
            char lowerC = tolower(c);
            if (lowerC == 'j')
                lowerC = 'i';
            text.push_back(lowerC);
        }
    }
    // Create digraphs, inserting 'x' between identical letters or at end.
    vector<string> digraphs;
    for (size_t i = 0; i < text.size(); ) {
        char first = text[i];
        char second;
        if (i + 1 < text.size()) {
            second = text[i + 1];
            if (first == second) {
                second = 'x';
                i++;  // only one character consumed.
            } else {
                i += 2;
            }
        } else {
            second = 'x';
            i++;
        }
        digraphs.push_back(string()+first+second);
    }
    
    string result;
    // Encrypt each digraph.
    for (auto &pair : digraphs) {
        int index1 = find(matrix.begin(), matrix.end(), pair[0]) - matrix.begin();
        int index2 = find(matrix.begin(), matrix.end(), pair[1]) - matrix.begin();
        int row1 = index1 / 5, col1 = index1 % 5;
        int row2 = index2 / 5, col2 = index2 % 5;
        
        if (row1 == row2) {
            // Same row: shift right.
            result.push_back(matrix[row1 * 5 + (col1 + 1) % 5]);
            result.push_back(matrix[row2 * 5 + (col2 + 1) % 5]);
        } else if (col1 == col2) {
            // Same column: shift down.
            result.push_back(matrix[((row1 + 1) % 5) * 5 + col1]);
            result.push_back(matrix[((row2 + 1) % 5) * 5 + col2]);
        } else {
            // Rectangle: swap columns.
            result.push_back(matrix[row1 * 5 + col2]);
            result.push_back(matrix[row2 * 5 + col1]);
        }
    }
    return result;
}

/*****************************
 * Rail Fence Cipher
 *****************************/
string railFenceCipher(const string &plaintext, int rails) {
    if (rails <= 1) return plaintext;
    vector<string> fence(rails, "");
    int rail = 0;
    int direction = 1; // 1 means moving down, -1 means moving up.
    for (char c : plaintext) {
        fence[rail].push_back(c);
        rail += direction;
        if (rail == 0 || rail == rails - 1)
            direction = -direction;
    }
    string result;
    for (const auto &row : fence)
        result += row;
    return result;
}

/*****************************
 * Row Transposition Cipher
 *****************************/
string rowTranspositionCipher(const string &plaintext, const string &key) {
    int n = key.size();
    // Calculate number of rows.
    int rows = ceil((double)plaintext.size() / n);
    // Pad the plaintext with 'X' if necessary.
    string padded = plaintext;
    while (padded.size() < rows * n)
        padded.push_back('X');
    
    // Create columns.
    vector<string> grid(n, "");
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < n; c++) {
            grid[c].push_back(padded[r * n + c]);
        }
    }
    
    // The key is expected as a permutation (e.g., "3124").
    // We need to output columns in the order indicated by the numeric key.
    string result;
    // Create a vector of pairs: (key_digit, original_index)
    vector<pair<int,int>> order;
    for (int i = 0; i < n; i++) {
        order.push_back({ key[i] - '0', i });
    }
    sort(order.begin(), order.end());
    for (auto &p : order) {
        result += grid[p.second];
    }
    return result;
}

/*****************************
 * Main Program
 *****************************/
int main() {
    int choice;
    cout << "Basic Cryptographic Techniques Lab (Lab 05)\n";
    cout << "By: Venkatesan M (22BAI1259)\n\n";
    cout << "Select the cipher to use:\n";
    cout << "1. Caesar Cipher\n";
    cout << "2. Vigenère Cipher\n";
    cout << "3. Hill Cipher\n";
    cout << "4. Playfair Cipher\n";
    cout << "5. Rail Fence Cipher\n";
    cout << "6. Row Transposition Cipher\n";
    cout << "Enter your choice (1-6): ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // Caesar Cipher
        string plaintext;
        int shift;
        cout << "Enter the text to encrypt using Caesar Cipher: ";
        getline(cin, plaintext);
        cout << "Enter the shift value: ";
        cin >> shift;
        string ciphertext = caesarCipher(plaintext, shift);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else if (choice == 2) {
        // Vigenère Cipher
        string plaintext, key;
        cout << "Enter the text to encrypt using Vigenère Cipher: ";
        getline(cin, plaintext);
        cout << "Enter the key for Vigenère Cipher: ";
        getline(cin, key);
        string ciphertext = vigenereCipher(plaintext, key);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else if (choice == 3) {
        // Hill Cipher
        string plaintext;
        cout << "Enter the text to encrypt using Hill Cipher: ";
        getline(cin, plaintext);
        vector<vector<int>> keyMatrix = {
            {6, 24, 1},
            {13, 16, 10},
            {20, 17, 15}
        };
        string ciphertext = hillCipher(plaintext, keyMatrix);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else if (choice == 4) {
        // Playfair Cipher
        string plaintext, key;
        cout << "Enter the text to encrypt using Playfair Cipher: ";
        getline(cin, plaintext);
        cout << "Enter the key for Playfair Cipher: ";
        getline(cin, key);
        string ciphertext = playfairCipher(plaintext, key);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else if (choice == 5) {
        // Rail Fence Cipher
        string plaintext;
        int rails;
        cout << "Enter the text to encrypt using Rail Fence Cipher: ";
        getline(cin, plaintext);
        cout << "Enter the number of rails: ";
        cin >> rails;
        string ciphertext = railFenceCipher(plaintext, rails);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else if (choice == 6) {
        // Row Transposition Cipher
        string plaintext, key;
        cout << "Enter the text to encrypt using Row Transposition Cipher: ";
        getline(cin, plaintext);
        cout << "Enter the key for Row Transposition Cipher (e.g., \"3124\"): ";
        getline(cin, key);
        string ciphertext = rowTranspositionCipher(plaintext, key);
        cout << "Ciphertext: " << ciphertext << endl;
    }
    else {
        cout << "Invalid choice. Exiting." << endl;
    }
    
    return 0;
}
