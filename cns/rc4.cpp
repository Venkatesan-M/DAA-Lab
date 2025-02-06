#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

/**
 * RC4 encryption/decryption function.
 * Since RC4 is symmetric, the same function is used for both encryption and decryption.
 *
 * @param key The key string (expected to be 32 characters, representing 256 bits).
 * @param input The plaintext (for encryption) or ciphertext (for decryption).
 * @return The resulting ciphertext or decrypted text.
 */
string rc4(const string &key, const string &input) {
    // Key Scheduling Algorithm (KSA)
    vector<int> S(256);
    int key_length = key.size();
    for (int i = 0; i < 256; i++) {
        S[i] = i;
    }
    
    int j = 0;
    for (int i = 0; i < 256; i++) {
        // Use the ASCII value of the key's character in the summing process.
        j = (j + S[i] + static_cast<unsigned char>(key[i % key_length])) % 256;
        swap(S[i], S[j]);
    }
    
    // Pseudo-Random Generation Algorithm (PRGA)
    string output;
    output.resize(input.size());
    int i = 0, j2 = 0;
    
    for (size_t k = 0; k < input.size(); k++) {
        i = (i + 1) % 256;
        j2 = (j2 + S[i]) % 256;
        swap(S[i], S[j2]);
        int t = (S[i] + S[j2]) % 256;
        int K = S[t];
        // XOR the keystream with the input byte (works for encryption and decryption)
        output[k] = input[k] ^ K;
    }
    
    return output;
}

int main() {
    string plaintext;
    string key;
    
    // Get plaintext from the user.
    cout << "Enter the plaintext to encrypt: ";
    getline(cin, plaintext);
    
    // Get the key from the user.
    cout << "Enter the key for RC4 encryption (256-bit key, 32 characters): ";
    getline(cin, key);
    
    // Validate key length.
    if (key.size() != 32) {
        cerr << "Error: The key must be exactly 32 characters long." << endl;
        return 1;
    }
    
    cout << "\nOriginal Plaintext: " << plaintext << endl;
    cout << "Encryption Key: " << key << "\n" << endl;
    
    // Encrypt the plaintext.
    string ciphertext = rc4(key, plaintext);
    
    // Display the ciphertext as hexadecimal values.
    cout << "Ciphertext (Encrypted in hex): ";
    for (unsigned char c : ciphertext) {
        // Print each byte as a two-digit hexadecimal number.
        cout << "0x" 
             << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << "\n" << endl;
    
    // Decrypt the ciphertext.
    string decrypted_text = rc4(key, ciphertext);
    cout << "Decrypted Text: " << decrypted_text << endl;
    
    return 0;
}
