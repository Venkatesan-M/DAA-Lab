#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cmath>
#include <tuple>
#include <limits>
#include <utility>

// Function to calculate (base^exponent) % modulus efficiently
long long power_mod(long long base, long long exponent, long long modulus) {
    long long result = 1;
    base = base % modulus;
    
    while (exponent > 0) {
        // If exponent is odd, multiply result with base
        if (exponent % 2 == 1)
            result = (result * base) % modulus;
            
        // Exponent must be even now
        exponent = exponent >> 1; // Divide by 2
        base = (base * base) % modulus; // Square the base
    }
    
    return result;
}

// Function to calculate greatest common divisor
long long gcd(long long a, long long b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Function to perform extended Euclidean algorithm to find modular inverse
long long modular_inverse(long long a, long long m) {
    long long m0 = m;
    long long y = 0, x = 1;
    
    if (m == 1)
        return 0;
        
    while (a > 1) {
        // q is quotient
        long long q = a / m;
        long long t = m;
        
        // m is remainder now, process same as Euclid's algorithm
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

// Function to check if a number is prime (simple implementation)
bool is_prime(long long n) {
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;
        
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    
    return true;
}

// Function to generate a random prime number in the range [min, max]
long long generate_prime(long long min, long long max) {
    long long rand_num;
    do {
        rand_num = min + rand() % (max - min + 1);
    } while (!is_prime(rand_num));
    
    return rand_num;
}

// Function to find a generator of a prime modulus
long long find_generator(long long p) {
    // In practice, this is a complex operation
    // For simplicity, we'll test small values
    long long phi = p - 1;
    
    for (long long g = 2; g < p; g++) {
        if (gcd(g, p) == 1 && power_mod(g, phi, p) == 1) {
            bool is_generator = true;
            
            // Check if g^k mod p = 1 for any k < phi
            for (long long k = 1; k < phi; k++) {
                if (phi % k == 0 && power_mod(g, k, p) == 1) {
                    is_generator = false;
                    break;
                }
            }
            
            if (is_generator)
                return g;
        }
    }
    
    return -1; // No generator found
}

// Convert string message to a numerical value
long long string_to_number(const std::string& message) {
    long long result = 0;
    
    for (char c : message) {
        result = result * 256 + (unsigned char)c;
    }
    
    return result;
}

class ElGamalDSA {
private:
    long long p; // Large prime number
    long long g; // Generator of p
    long long x; // Private key (x < p)
    long long y; // Public key (y = g^x mod p)
    
public:
    // Constructor
    ElGamalDSA() : p(0), g(0), x(0), y(0) {}
    
    // Generate key pair
    void generate_keys() {
        std::cout << "Generating keys...\n";
        
        // For simplicity, let's use a small prime range
        p = generate_prime(100, 1000);
        std::cout << "Prime (p): " << p << std::endl;
        
        // Find a generator g
        g = find_generator(p);
        if (g == -1) {
            // If no generator found, use a simpler approach
            g = 2;
            while (power_mod(g, (p-1)/2, p) == 1) {
                g++;
            }
        }
        std::cout << "Generator (g): " << g << std::endl;
        
        // Generate private key x
        x = 1 + rand() % (p - 2); // 1 <= x <= p-2
        std::cout << "Private key (x): " << x << std::endl;
        
        // Calculate public key y = g^x mod p
        y = power_mod(g, x, p);
        std::cout << "Public key (y): " << y << std::endl;
    }
    
    // Set keys manually
    void set_keys(long long new_p, long long new_g, long long new_x) {
        p = new_p;
        g = new_g;
        x = new_x;
        y = power_mod(g, x, p);
    }
    
    // Sign a message
    std::pair<long long, long long> sign(const std::string& message) {
        // Convert message to a number
        long long m = string_to_number(message);
        
        // Choose a random k, where 1 <= k <= p-2 and gcd(k, p-1) = 1
        long long k;
        do {
            k = 1 + rand() % (p - 2);
        } while (gcd(k, p-1) != 1);
        
        // Calculate r = g^k mod p
        long long r = power_mod(g, k, p);
        
        // Calculate s = (m - x*r) * k^-1 mod (p-1)
        long long k_inv = modular_inverse(k, p-1);
        long long s = ((m - x * r) % (p-1) + (p-1)) % (p-1);
        s = (s * k_inv) % (p-1);
        
        return std::make_pair(r, s);
    }
    
    // Verify a signature
    bool verify(const std::string& message, long long r, long long s) {
        // Check if 0 < r < p and 0 < s < p-1
        if (r <= 0 || r >= p || s <= 0 || s >= p-1)
            return false;
            
        // Convert message to a number
        long long m = string_to_number(message);
        
        // Calculate v1 = g^m mod p
        long long v1 = power_mod(g, m, p);
        
        // Calculate v2 = (y^r * r^s) mod p
        long long v2 = (power_mod(y, r, p) * power_mod(r, s, p)) % p;
        
        // Signature is valid if v1 = v2
        return v1 == v2;
    }
    
    // Get public components for verification
    std::tuple<long long, long long, long long> get_public_components() {
        return std::make_tuple(p, g, y);
    }
};

int main() {
    // Set random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    ElGamalDSA elgamal;
    int choice;
    
    std::cout << "ElGamal Digital Signature Algorithm\n";
    std::cout << "===================================\n";
    
    while (true) {
        std::cout << "\nChoose an option:\n";
        std::cout << "1. Generate keys\n";
        std::cout << "2. Set keys manually\n";
        std::cout << "3. Sign a message\n";
        std::cout << "4. Verify a signature\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                elgamal.generate_keys();
                break;
            }
            case 2: {
                long long p, g, x;
                std::cout << "Enter prime (p): ";
                std::cin >> p;
                std::cout << "Enter generator (g): ";
                std::cin >> g;
                std::cout << "Enter private key (x): ";
                std::cin >> x;
                elgamal.set_keys(p, g, x);
                
                auto [pub_p, pub_g, pub_y] = elgamal.get_public_components();
                std::cout << "Public key (y): " << pub_y << std::endl;
                
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            case 3: {
                std::string message;
                std::cout << "Enter message to sign: ";
                std::getline(std::cin, message);
                
                auto [r, s] = elgamal.sign(message);
                std::cout << "Signature: (r, s) = (" << r << ", " << s << ")\n";
                break;
            }
            case 4: {
                std::string message;
                long long r, s;
                std::cout << "Enter message to verify: ";
                std::getline(std::cin, message);
                std::cout << "Enter signature component r: ";
                std::cin >> r;
                std::cout << "Enter signature component s: ";
                std::cin >> s;
                
                bool is_valid = elgamal.verify(message, r, s);
                if (is_valid)
                    std::cout << "Signature is valid!\n";
                else
                    std::cout << "Signature is invalid!\n";
                
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            case 5:
                std::cout << "Exiting program.\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}