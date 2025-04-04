#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <random>
#include <limits>

// Structure to hold public and private keys
struct RSAKey {
    long long n;      // Modulus (n = p * q)
    long long e;      // Public exponent
    long long d;      // Private exponent
};

// Function to check if a number is prime
bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    
    return true;
}

// Function to generate a random prime number within a range
long long generateRandomPrime(long long min, long long max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dist(min, max);
    
    long long num;
    do {
        num = dist(gen);
    } while (!isPrime(num));
    
    return num;
}

// Function to calculate GCD using Euclidean algorithm
long long gcd(long long a, long long b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Function to calculate modular multiplicative inverse using Extended Euclidean Algorithm
long long modInverse(long long a, long long m) {
    long long m0 = m;
    long long y = 0, x = 1;
    
    if (m == 1)
        return 0;
    
    while (a > 1) {
        // q is quotient
        long long q = a / m;
        long long t = m;
        
        // m is remainder now, process same as Euclid's algo
        m = a % m;
        a = t;
        t = y;
        
        // Update y and x
        y = x - q * y;
        x = t;
    }
    
    // Make x positive
    if (x < 0)
        x += m0;
    
    return x;
}

// Improved function to perform modular exponentiation efficiently (a^b mod n)
// Handles large numbers better to avoid overflow
long long modPow(long long base, long long exponent, long long modulus) {
    // Edge case
    if (modulus == 1) return 0;
    
    // Initialize result
    long long result = 1;
    
    // Update base if it is more than or equal to modulus
    base = base % modulus;
    
    while (exponent > 0) {
        // If exponent is odd, multiply result with base
        if (exponent % 2 == 1)
            result = (result * base) % modulus;
        
        // Exponent must be even now
        exponent = exponent >> 1; // Divide by 2
        base = ((base % modulus) * (base % modulus)) % modulus;
    }
    
    return result;
}

// Function to generate RSA key pair
RSAKey generateRSAKeyPair() {
    // Use smaller primes to avoid overflow with long long
    long long p = generateRandomPrime(50, 100);
    long long q = generateRandomPrime(50, 100);
    while (p == q) {
        q = generateRandomPrime(50, 100);
    }
    
    // Step 2: Compute n = p * q
    long long n = p * q;
    
    // Step 3: Compute phi(n) = (p-1) * (q-1)
    long long phi = (p - 1) * (q - 1);
    
    // Step 4: Choose e such that 1 < e < phi and gcd(e, phi) = 1
    // Using a smaller e value to avoid potential issues
    long long e = 3;
    while (gcd(e, phi) != 1) {
        e += 2;
    }
    
    // Step 5: Compute d such that (d * e) % phi = 1
    long long d = modInverse(e, phi);
    
    RSAKey key;
    key.n = n;
    key.e = e;
    key.d = d;
    
    std::cout << "Generated RSA key pair:\n";
    std::cout << "p = " << p << ", q = " << q << "\n";
    std::cout << "n = " << n << "\n";
    std::cout << "phi(n) = " << phi << "\n";
    std::cout << "e = " << e << "\n";
    std::cout << "d = " << d << "\n";
    
    return key;
}

// Simple hash function (for demonstration purposes only)
// Using a smaller range to avoid overflow issues
long long simpleHash(const std::string& message) {
    // Use a smaller modulus to avoid overflow
    const long long mod = 10000;
    long long hash = 0;
    
    for (char c : message) {
        hash = (hash * 13 + c) % mod;
    }
    
    return hash;
}

// Function to sign a message using RSA private key
long long signMessage(const std::string& message, const RSAKey& key) {
    // Step 1: Compute the hash of the message
    long long hash = simpleHash(message);
    std::cout << "Message hash: " << hash << "\n";
    
    // Step 2: Compute signature = hash^d mod n
    long long signature = modPow(hash, key.d, key.n);
    
    return signature;
}

// Function to verify a signature using RSA public key
bool verifySignature(const std::string& message, long long signature, const RSAKey& key) {
    // Step 1: Compute the hash of the message
    long long hash = simpleHash(message);
    std::cout << "Verification hash: " << hash << "\n";
    
    // Step 2: Compute hash' = signature^e mod n
    long long decryptedHash = modPow(signature, key.e, key.n);
    std::cout << "Decrypted hash: " << decryptedHash << "\n";
    
    // Step 3: Compare hash and hash'
    return hash == decryptedHash;
}

// Function to clear input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::cout << "=== RSA Digital Signature Implementation ===\n\n";
    
    // Generate RSA key pair
    std::cout << "Generating RSA keys...\n";
    RSAKey key = generateRSAKeyPair();
    std::cout << "\n";
    
    int choice = 0;
    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Sign a message\n";
        std::cout << "2. Verify a signature\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice (1-3): ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }
        
        clearInputBuffer();
        
        if (choice == 1) {
            // Sign a message
            std::string message;
            std::cout << "\nEnter the message to sign: ";
            std::getline(std::cin, message);
            
            long long signature = signMessage(message, key);
            std::cout << "Signature generated: " << signature << "\n";
            
            // Save for verification
            std::cout << "\nKeep these values for verification:\n";
            std::cout << "Message: " << message << "\n";
            std::cout << "Signature: " << signature << "\n";
            std::cout << "Public Key (n, e): (" << key.n << ", " << key.e << ")\n";
            
        } else if (choice == 2) {
            // Verify a signature
            std::string message;
            long long signature;
            long long n, e;
            
            std::cout << "\nEnter the message to verify: ";
            std::getline(std::cin, message);
            
            std::cout << "Enter the signature: ";
            if (!(std::cin >> signature)) {
                std::cout << "Invalid signature input.\n";
                clearInputBuffer();
                continue;
            }
            
            std::cout << "Enter the modulus (n): ";
            if (!(std::cin >> n)) {
                std::cout << "Invalid modulus input.\n";
                clearInputBuffer();
                continue;
            }
            
            std::cout << "Enter the public exponent (e): ";
            if (!(std::cin >> e)) {
                std::cout << "Invalid exponent input.\n";
                clearInputBuffer();
                continue;
            }
            
            // Create a key with just the public components
            RSAKey verifyKey;
            verifyKey.n = n;
            verifyKey.e = e;
            verifyKey.d = 0; // Not needed for verification
            
            // Verify
            bool isValid = verifySignature(message, signature, verifyKey);
            std::cout << "\nSignature verification result: " << (isValid ? "Valid ✓" : "Invalid ✗") << "\n";
            
            clearInputBuffer();
            
        } else if (choice == 3) {
            std::cout << "\nExiting program. Goodbye!\n";
            break;
        } else {
            std::cout << "\nInvalid choice. Please select 1-3.\n";
        }
        
    } while (choice != 3);
    
    return 0;
}