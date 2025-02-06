#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <cassert>

using namespace std;

const size_t BLOCK_SIZE = 16; // 16 bytes per block

// *******************************
// Simple block cipher algorithm
// *******************************
// For demonstration, our "block cipher" takes a 16-byte block and a 16-byte key.
// Encryption: Each byte is added (mod 256) with the corresponding key byte.
vector<unsigned char> simpleBlockEncrypt(const vector<unsigned char>& block, const vector<unsigned char>& key) {
    assert(block.size() == BLOCK_SIZE && key.size() == BLOCK_SIZE);
    vector<unsigned char> out(BLOCK_SIZE);
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (block[i] + key[i]) % 256;
    }
    return out;
}

// Decryption: Subtract the key bytes modulo 256.
vector<unsigned char> simpleBlockDecrypt(const vector<unsigned char>& block, const vector<unsigned char>& key) {
    assert(block.size() == BLOCK_SIZE && key.size() == BLOCK_SIZE);
    vector<unsigned char> out(BLOCK_SIZE);
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (block[i] + 256 - key[i]) % 256;
    }
    return out;
}

// *******************************
// Utility functions
// *******************************
// Convert a string to a vector of unsigned char
vector<unsigned char> stringToBytes(const string &s) {
    return vector<unsigned char>(s.begin(), s.end());
}

// Pad the plaintext to a multiple of BLOCK_SIZE (simple zero padding)
vector<unsigned char> padPlaintext(const vector<unsigned char>& pt) {
    vector<unsigned char> padded = pt;
    size_t padLen = BLOCK_SIZE - (padded.size() % BLOCK_SIZE);
    if (padLen != BLOCK_SIZE) { // if already multiple, no padding is added
        padded.insert(padded.end(), padLen, 0);
    }
    return padded;
}

// Convert bytes to a hex string
string bytesToHex(const vector<unsigned char>& data) {
    stringstream ss;
    for (auto byte : data) {
        ss << hex << setw(2) << setfill('0') << (int)byte;
    }
    return ss.str();
}

// Increment a 16-byte counter (treating it as a little-endian integer)
void incrementCounter(vector<unsigned char>& counter) {
    for (size_t i = 0; i < counter.size(); i++) {
        if (++counter[i] != 0) break;
    }
}

// *******************************
// Block Cipher Modes
// *******************************

// 1. ECB (Electronic Code Book)
vector<unsigned char> encryptECB(const vector<unsigned char>& plaintext, const vector<unsigned char>& key) {
    vector<unsigned char> padded = padPlaintext(plaintext);
    vector<unsigned char> ciphertext;
    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + BLOCK_SIZE);
        vector<unsigned char> encBlock = simpleBlockEncrypt(block, key);
        ciphertext.insert(ciphertext.end(), encBlock.begin(), encBlock.end());
    }
    return ciphertext;
}

// 2. CBC (Cipher Block Chaining)
vector<unsigned char> encryptCBC(const vector<unsigned char>& plaintext, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    vector<unsigned char> padded = padPlaintext(plaintext);
    vector<unsigned char> ciphertext;
    vector<unsigned char> prev = iv;  // initialize with IV
    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + BLOCK_SIZE);
        // XOR block with previous ciphertext (or IV for the first block)
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            block[j] = block[j] ^ prev[j];
        }
        vector<unsigned char> encBlock = simpleBlockEncrypt(block, key);
        ciphertext.insert(ciphertext.end(), encBlock.begin(), encBlock.end());
        prev = encBlock;
    }
    return ciphertext;
}

// 3. OFB (Output Feedback)
// We generate a keystream by repeatedly encrypting the IV.
vector<unsigned char> encryptOFB(const vector<unsigned char>& plaintext, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    vector<unsigned char> padded = padPlaintext(plaintext);
    vector<unsigned char> ciphertext;
    vector<unsigned char> out = iv; // initial output feedback
    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        out = simpleBlockEncrypt(out, key); // update the keystream block
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + BLOCK_SIZE);
        // XOR plaintext block with the keystream
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            block[j] = block[j] ^ out[j];
        }
        ciphertext.insert(ciphertext.end(), block.begin(), block.end());
    }
    return ciphertext;
}

// 4. CFB (Cipher Feedback)
// Similar to OFB except that the cipher output is derived from the previous ciphertext.
vector<unsigned char> encryptCFB(const vector<unsigned char>& plaintext, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    vector<unsigned char> padded = padPlaintext(plaintext);
    vector<unsigned char> ciphertext;
    vector<unsigned char> prev = iv;
    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        vector<unsigned char> keystream = simpleBlockEncrypt(prev, key);
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + BLOCK_SIZE);
        // XOR plaintext block with keystream
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            block[j] = block[j] ^ keystream[j];
        }
        ciphertext.insert(ciphertext.end(), block.begin(), block.end());
        prev = block;  // update previous with the ciphertext block
    }
    return ciphertext;
}

// 5. GCM (Galois/Counter Mode) – Simplified demonstration.
// Here we use a counter mode encryption (similar to CTR mode) and produce a dummy authentication tag.
pair<vector<unsigned char>, vector<unsigned char>> encryptGCM(const vector<unsigned char>& plaintext, const vector<unsigned char>& key, vector<unsigned char> iv) {
    vector<unsigned char> padded = padPlaintext(plaintext);
    vector<unsigned char> ciphertext;
    // In GCM the counter is derived from the IV. We make a local copy.
    vector<unsigned char> counter = iv;
    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        vector<unsigned char> keystream = simpleBlockEncrypt(counter, key);
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + BLOCK_SIZE);
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            block[j] = block[j] ^ keystream[j];
        }
        ciphertext.insert(ciphertext.end(), block.begin(), block.end());
        incrementCounter(counter);  // increment counter for next block
    }
    // For demonstration, we create a dummy authentication tag:
    // (e.g. XOR of all ciphertext bytes, repeated for BLOCK_SIZE)
    unsigned char tagVal = 0;
    for (auto byte : ciphertext) {
        tagVal ^= byte;
    }
    vector<unsigned char> authTag(BLOCK_SIZE, tagVal);
    return {ciphertext, authTag};
}

// *******************************
// Main Program
// *******************************
int main() {
    cout << "Lab 04 (Block Cipher Modes of Operation)\n";
    cout << "Assignment by Venkatesan M (22BAI1259)\n\n";

    string plaintext, keyStr, ivStr;
    
    cout << "Enter the plaintext: ";
    getline(cin, plaintext);
    
    cout << "Enter the 16-character key: ";
    getline(cin, keyStr);
    if(keyStr.size() != BLOCK_SIZE) {
        cerr << "Error: Key must be exactly 16 characters long.\n";
        return 1;
    }
    
    cout << "Enter the 16-character IV: ";
    getline(cin, ivStr);
    if(ivStr.size() != BLOCK_SIZE) {
        cerr << "Error: IV must be exactly 16 characters long.\n";
        return 1;
    }
    
    // Convert inputs to byte vectors.
    vector<unsigned char> ptBytes = stringToBytes(plaintext);
    vector<unsigned char> keyBytes = stringToBytes(keyStr);
    vector<unsigned char> ivBytes = stringToBytes(ivStr);
    
    // Encrypt using different modes:
    vector<unsigned char> ecbCipher = encryptECB(ptBytes, keyBytes);
    vector<unsigned char> cbcCipher = encryptCBC(ptBytes, keyBytes, ivBytes);
    vector<unsigned char> ofbCipher = encryptOFB(ptBytes, keyBytes, ivBytes);
    vector<unsigned char> cfbCipher = encryptCFB(ptBytes, keyBytes, ivBytes);
    auto gcmResult = encryptGCM(ptBytes, keyBytes, ivBytes);
    vector<unsigned char> gcmCipher = gcmResult.first;
    vector<unsigned char> gcmTag = gcmResult.second;
    
    // Output results as hex strings.
    cout << "\n----- ECB Mode -----\n";
    cout << "Ciphertext (hex): " << bytesToHex(ecbCipher) << "\n";
    
    cout << "\n----- CBC Mode -----\n";
    cout << "Ciphertext (hex): " << bytesToHex(cbcCipher) << "\n";
    
    cout << "\n----- OFB Mode -----\n";
    cout << "Ciphertext (hex): " << bytesToHex(ofbCipher) << "\n";
    
    cout << "\n----- CFB Mode -----\n";
    cout << "Ciphertext (hex): " << bytesToHex(cfbCipher) << "\n";
    
    cout << "\n----- GCM Mode -----\n";
    cout << "Ciphertext (hex): " << bytesToHex(gcmCipher) << "\n";
    cout << "Authentication Tag (hex): " << bytesToHex(gcmTag) << "\n";
    
    return 0;
}
