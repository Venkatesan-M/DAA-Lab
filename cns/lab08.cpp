/*
 * Lab 08: ElGamal and Multiparty Diffie–Hellman Key Exchange Demo
 *
 * This program is divided into two parts:
 *
 * 1. ElGamal Encryption/Decryption:
 *    - Public parameters: a prime p and a generator g.
 *    - Key generation: A private key 'a' and public key h = g^a mod p.
 *    - Encryption: For a message m (m < p), choose a random ephemeral key k.
 *                   Compute ciphertext (c1, c2):
 *                       c1 = g^k mod p
 *                       c2 = m * (h^k mod p) mod p
 *    - Decryption: Compute s = c1^a mod p, then compute the modular inverse s_inv.
 *                   Recover m as: m = (c2 * s_inv) mod p.
 *
 * 2. Multiparty Diffie–Hellman Key Exchange (3 parties: A, B, C):
 *    This demonstration uses a simple “chain” protocol over three rounds:
 *      - Round 1: Each party computes and “sends” its public value:
 *                   A_pub = g^(a_A),  B_pub = g^(a_B),  C_pub = g^(a_C)
 *      - Round 2: Each party raises the public value received from the previous
 *                 party (in a circle) to its own secret:
 *                   Y_A = (C_pub)^(a_A), Y_B = (A_pub)^(a_B), Y_C = (B_pub)^(a_C)
 *      - Round 3: Each party raises the intermediate value received in Round 2
 *                 to its own secret, yielding:
 *                   K_A = (Y_C)^(a_A), K_B = (Y_A)^(a_B), K_C = (Y_B)^(a_C)
 *
 *    It can be verified that:
 *         K_A = K_B = K_C = g^(a_A * a_B * a_C) mod p,
 *    which is the shared secret key.
 *
 */

#include <iostream>
#include <cstdlib>

// Function to perform modular exponentiation:
// Computes (base^exp) mod mod efficiently.
long long modexp(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    while(exp > 0) {
        if(exp & 1)
            result = (result * base) % mod;
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

// Extended Euclidean Algorithm to compute gcd and coefficients
long long egcd(long long a, long long b, long long &x, long long &y) {
    if(b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long d = egcd(b, a % b, y, x);
    y -= (a / b) * x;
    return d;
}

// Function to compute the modular inverse of a modulo mod.
long long modinv(long long a, long long mod) {
    long long x, y;
    long long g = egcd(a, mod, x, y);
    if(g != 1) {
        std::cerr << "Error: Modular inverse does not exist!" << std::endl;
        exit(1);
    }
    x = (x % mod + mod) % mod;
    return x;
}

int main() {
    // ---------------- Public Parameters ----------------
    // For demonstration we use small numbers.
    long long p = 467;   // A prime number
    long long g = 2;     // A generator for the multiplicative group modulo p

    std::cout << "Public parameters: p = " << p << ", g = " << g << "\n\n";

    // ===================== ElGamal Encryption Demo =====================
    std::cout << "----- ElGamal Encryption Demo -----\n";

    // Key generation: choose a private key 'a'
    long long a = 127;    // Private key (for demo, fixed value)
    long long h = modexp(g, a, p);  // Public key: h = g^a mod p

    std::cout << "Private key (a): " << a << "\n";
    std::cout << "Public key (h = g^a mod p): " << h << "\n";

    // The message to encrypt (should be less than p)
    long long m = 88;
    std::cout << "Original message: " << m << "\n";

    // Encryption: choose a random ephemeral key k
    long long k = 53;  // For demo purposes, using a fixed k
    long long c1 = modexp(g, k, p);              // c1 = g^k mod p
    long long c2 = (m * modexp(h, k, p)) % p;      // c2 = m * h^k mod p

    std::cout << "Random ephemeral key (k): " << k << "\n";
    std::cout << "Ciphertext: (c1, c2) = (" << c1 << ", " << c2 << ")\n";

    // Decryption:
    // Compute s = c1^a mod p and then its modular inverse.
    long long s = modexp(c1, a, p);
    long long s_inv = modinv(s, p);
    long long decrypted = (c2 * s_inv) % p;

    std::cout << "Decrypted message: " << decrypted << "\n\n";

    // ===================== Multiparty Diffie–Hellman Demo =====================
    std::cout << "----- Multiparty Diffie-Hellman (3 parties) Demo -----\n";

    // Assume three parties: A, B, and C.
    // Each party chooses a secret exponent.
    long long a_A = 23;  // Secret of party A
    long long a_B = 17;  // Secret of party B
    long long a_C = 31;  // Secret of party C

    std::cout << "Party A secret: " << a_A << "\n";
    std::cout << "Party B secret: " << a_B << "\n";
    std::cout << "Party C secret: " << a_C << "\n";

    // Round 1: Each party computes its public value.
    long long A_pub = modexp(g, a_A, p);  // A computes g^(a_A)
    long long B_pub = modexp(g, a_B, p);  // B computes g^(a_B)
    long long C_pub = modexp(g, a_C, p);  // C computes g^(a_C)

    // Round 2: Each party raises the public value received from the previous party.
    // For a circular ordering A -> B -> C -> A:
    long long Y_A = modexp(C_pub, a_A, p);  // A computes (C_pub)^(a_A) = g^(a_C * a_A)
    long long Y_B = modexp(A_pub, a_B, p);  // B computes (A_pub)^(a_B) = g^(a_A * a_B)
    long long Y_C = modexp(B_pub, a_C, p);  // C computes (B_pub)^(a_C) = g^(a_B * a_C)

    // Round 3: Each party raises the intermediate value from the previous party to its own secret.
    long long K_A = modexp(Y_C, a_A, p);  // A computes: (g^(a_B*a_C))^(a_A) = g^(a_A * a_B * a_C)
    long long K_B = modexp(Y_A, a_B, p);  // B computes: (g^(a_C*a_A))^(a_B) = g^(a_A * a_B * a_C)
    long long K_C = modexp(Y_B, a_C, p);  // C computes: (g^(a_A*a_B))^(a_C) = g^(a_A * a_B * a_C)

    std::cout << "Shared key computed by Party A: " << K_A << "\n";
    std::cout << "Shared key computed by Party B: " << K_B << "\n";
    std::cout << "Shared key computed by Party C: " << K_C << "\n";

    if(K_A == K_B && K_B == K_C)
        std::cout << "All parties share the same key: " << K_A << "\n";
    else
        std::cout << "Error: Keys do not match!\n";

    return 0;
}
