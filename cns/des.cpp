#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

using namespace std;

typedef vector<int> BitVector;

// -------------------------------
// Helper Functions for Bit Vectors
// -------------------------------

// Convert a hexadecimal character to its integer value.
int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    c = toupper(c);
    return c - 'A' + 10;
}

// Convert a hex string (e.g., "0123456789ABCDEF") to a BitVector (each element is 0 or 1).
BitVector hexStringToBitVector(const string &hex) {
    BitVector bits;
    for (char c : hex) {
        int value = hexCharToInt(c);
        // Each hex digit represents 4 bits.
        for (int i = 3; i >= 0; i--) {
            bits.push_back((value >> i) & 1);
        }
    }
    return bits;
}

// Convert a BitVector (of 0’s and 1’s) to a hex string.
string bitVectorToHexString(const BitVector &bits) {
    assert(bits.size() % 4 == 0);
    stringstream ss;
    for (size_t i = 0; i < bits.size(); i += 4) {
        int value = 0;
        for (int j = 0; j < 4; j++) {
            value = (value << 1) | bits[i+j];
        }
        ss << std::hex << std::uppercase << value;
    }
    return ss.str();
}

// Permute a bit vector according to a given table.
BitVector permute(const BitVector &input, const vector<int> &table) {
    BitVector output;
    for (int pos : table) {
        // DES tables are 1-indexed so subtract 1.
        output.push_back(input[pos - 1]);
    }
    return output;
}

// Left shift a vector of bits by n positions (circular shift).
void leftShift(BitVector &bits, int n) {
    int size = bits.size();
    BitVector temp = bits;
    for (int i = 0; i < size; i++) {
        bits[i] = temp[(i + n) % size];
    }
}

// XOR two bit vectors (of the same length).
BitVector xorBits(const BitVector &a, const BitVector &b) {
    assert(a.size() == b.size());
    BitVector result(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

// -------------------------------
// DES Tables
// -------------------------------

// Initial Permutation Table (IP)
const vector<int> IP = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// Final Permutation Table (FP), which is the inverse of IP.
const vector<int> FP = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

// Expansion Table (E) expands 32 bits to 48 bits.
const vector<int> E = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

// S-boxes: There are eight S-boxes, each with 4 rows and 16 columns.
const int S_BOX[8][4][16] = {
    // S1
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    // S2
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    // S3
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    // S4
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    // S5
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    // S6
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    // S7
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    // S8
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

// Permutation P (32 bits)
const vector<int> P = {
    16,7,20,21,
    29,12,28,17,
    1,15,23,26,
    5,18,31,10,
    2,8,24,14,
    32,27,3,9,
    19,13,30,6,
    22,11,4,25
};

// Permuted Choice 1 (PC-1) for key schedule (56 bits)
const vector<int> PC1 = {
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
};

// Permuted Choice 2 (PC-2) for key schedule (48 bits)
const vector<int> PC2 = {
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
};

// Number of left shifts for each of the 16 rounds.
const vector<int> SHIFTS = {
    1, 1, 2, 2,
    2, 2, 2, 2,
    1, 2, 2, 2,
    2, 2, 2, 1
};

// -------------------------------
// Key Schedule: Generate 16 round keys (each 48 bits)
// -------------------------------
vector<BitVector> generateRoundKeys(const BitVector &key64) {
    // Apply PC-1 to get 56-bit key.
    BitVector key56 = permute(key64, PC1);
    // Split into left (C) and right (D) halves.
    BitVector C(key56.begin(), key56.begin() + 28);
    BitVector D(key56.begin() + 28, key56.end());
    
    vector<BitVector> roundKeys;
    for (int round = 0; round < 16; round++) {
        // Left shift both halves.
        leftShift(C, SHIFTS[round]);
        leftShift(D, SHIFTS[round]);
        // Combine halves.
        BitVector combined;
        combined.insert(combined.end(), C.begin(), C.end());
        combined.insert(combined.end(), D.begin(), D.end());
        // Apply PC-2 to get the 48-bit round key.
        BitVector roundKey = permute(combined, PC2);
        roundKeys.push_back(roundKey);
    }
    return roundKeys;
}

// -------------------------------
// DES Feistel (F) Function
// -------------------------------
BitVector feistel(const BitVector &R, const BitVector &roundKey) {
    // Expand R from 32 to 48 bits using expansion table E.
    BitVector expanded = permute(R, E);
    // XOR with round key.
    BitVector xored = xorBits(expanded, roundKey);
    
    // Apply the 8 S-boxes.
    BitVector sboxOutput;
    for (int i = 0; i < 8; i++) {
        // Each block is 6 bits.
        int offset = i * 6;
        int row = (xored[offset] << 1) | xored[offset + 5];
        int col = (xored[offset + 1] << 3) | (xored[offset + 2] << 2) | (xored[offset + 3] << 1) | xored[offset + 4];
        int s_val = S_BOX[i][row][col];
        // Convert s_val (0-15) to 4 bits.
        for (int j = 3; j >= 0; j--) {
            sboxOutput.push_back((s_val >> j) & 1);
        }
    }
    
    // Apply permutation P to the 32-bit result.
    BitVector fOutput = permute(sboxOutput, P);
    return fOutput;
}

// -------------------------------
// DES Encryption (one 64-bit block)
// -------------------------------
BitVector DES_Encrypt(const BitVector &plaintext, const vector<BitVector> &roundKeys) {
    // Initial Permutation.
    BitVector permutedBlock = permute(plaintext, IP);
    // Split into left (L) and right (R) halves (32 bits each).
    BitVector L(permutedBlock.begin(), permutedBlock.begin() + 32);
    BitVector R(permutedBlock.begin() + 32, permutedBlock.end());
    
    // 16 rounds.
    for (int round = 0; round < 16; round++) {
        BitVector previousR = R;
        // Feistel function.
        BitVector fOut = feistel(R, roundKeys[round]);
        // New R is L XOR f(R, roundKey)
        R = xorBits(L, fOut);
        L = previousR;
    }
    
    // Pre-output: combine R and L (note the swap).
    BitVector combined;
    combined.insert(combined.end(), R.begin(), R.end());
    combined.insert(combined.end(), L.begin(), L.end());
    // Apply final permutation.
    BitVector ciphertext = permute(combined, FP);
    return ciphertext;
}

// -------------------------------
// DES Decryption (one 64-bit block)
// (Same as encryption but round keys are used in reverse order.)
// -------------------------------
BitVector DES_Decrypt(const BitVector &ciphertext, const vector<BitVector> &roundKeys) {
    // Apply the initial permutation.
    BitVector permutedBlock = permute(ciphertext, IP);
    // Split into left and right halves.
    BitVector L(permutedBlock.begin(), permutedBlock.begin() + 32);
    BitVector R(permutedBlock.begin() + 32, permutedBlock.end());
    
    // Process 16 rounds in reverse order.
    for (int round = 15; round >= 0; round--) {
        BitVector temp = R;
        R = xorBits(L, feistel(R, roundKeys[round]));
        L = temp;
    }
    
    // Combine halves (note that we swap them as in encryption).
    BitVector combined;
    combined.insert(combined.end(), R.begin(), R.end());
    combined.insert(combined.end(), L.begin(), L.end());
    // Apply final permutation.
    return permute(combined, FP);
}



// -------------------------------
// Main Program
// -------------------------------
int main() {
    cout << "DES Encryption Implementation (by Venkatesan M, 22BAI1259)" << endl;
    cout << "Enter 16-hex-digit plaintext (64 bits): ";
    string plaintextHex;
    cin >> plaintextHex;
    
    cout << "Enter 16-hex-digit key (64 bits): ";
    string keyHex;
    cin >> keyHex;
    
    // Convert input hex strings to bit vectors.
    BitVector plaintextBits = hexStringToBitVector(plaintextHex);
    BitVector keyBits = hexStringToBitVector(keyHex);
    
    // Generate 16 round keys.
    vector<BitVector> roundKeys = generateRoundKeys(keyBits);
    
    // Encrypt the plaintext.
    BitVector ciphertextBits = DES_Encrypt(plaintextBits, roundKeys);
    string ciphertextHex = bitVectorToHexString(ciphertextBits);
    
    cout << "\nCiphertext: " << ciphertextHex << endl;
    
    // For demonstration, decrypt the ciphertext.
    BitVector decryptedBits = DES_Decrypt(ciphertextBits, roundKeys);
    string decryptedHex = bitVectorToHexString(decryptedBits);
    
    cout << "Decrypted (hex): " << decryptedHex << endl;
    
    return 0;
}
