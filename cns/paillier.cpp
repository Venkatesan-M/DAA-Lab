#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Define type alias for clarity (using long long for small numbers)
typedef long long ll;

// Efficient modular exponentiation: computes (base^exp) mod mod.
ll modExp(ll base, ll exp, ll mod) {
    ll result = 1;
    base %= mod;
    while(exp > 0) {
        if(exp & 1)
            result = (result * base) % mod;
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

// Compute the greatest common divisor.
ll gcd(ll a, ll b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Compute the least common multiple.
ll lcm(ll a, ll b) {
    return a / gcd(a, b) * b;
}

// Extended Euclidean algorithm to find x and y such that: a*x + b*y = gcd(a, b).
ll extended_gcd(ll a, ll b, ll &x, ll &y) {
    if(b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    ll d = extended_gcd(b, a % b, y, x);
    y -= (a / b) * x;
    return d;
}

// Compute modular inverse of a modulo mod.
ll modInverse(ll a, ll mod) {
    ll x, y;
    ll g = extended_gcd(a, mod, x, y);
    if(g != 1) {
        // Inverse does not exist if a and mod are not coprime.
        return -1;
    } else {
        x = (x % mod + mod) % mod;
        return x;
    }
}

// L(u) function defined in Paillier cryptosystem.
ll L_function(ll u, ll n) {
    return (u - 1) / n;
}

int main() {
    // -------------------------------
    // Key Generation
    // -------------------------------
    // For demonstration, use small primes
    ll p = 11, q = 13;
    ll n = p * q;         // n = 11 * 13 = 143
    ll n_sq = n * n;      // n^2 = 20449
    ll lambda = lcm(p - 1, q - 1); // lambda = lcm(10, 12) = 60

    // Choose g = n + 1 (a common valid choice)
    ll g = n + 1;         // g = 144

    // Compute μ = (L(g^lambda mod n^2))^(-1) mod n
    ll g_lambda = modExp(g, lambda, n_sq); // g^λ mod n²
    ll L_val = L_function(g_lambda, n);      // L(g^λ mod n²)
    ll mu = modInverse(L_val, n);
    if(mu == -1) {
        cout << "Error: modular inverse of L(g^λ) does not exist." << endl;
        return 1;
    }

    // Display public and private keys
    cout << "Public key (n, g): (" << n << ", " << g << ")" << endl;
    cout << "Private key (lambda, mu): (" << lambda << ", " << mu << ")" << endl;

    // -------------------------------
    // Encryption
    // -------------------------------
    ll m;
    cout << "Enter plaintext m (0 <= m < " << n << "): ";
    cin >> m;
    if(m < 0 || m >= n) {
        cout << "Invalid plaintext. It must be in the range [0, " << n-1 << "]." << endl;
        return 1;
    }

    // Choose a random r in Z*_n (i.e., 1 <= r < n with gcd(r, n) = 1)
    srand(time(0));
    ll r;
    do {
        r = rand() % n;
    } while(r == 0 || gcd(r, n) != 1);
    cout << "Random r chosen: " << r << endl;

    // Compute ciphertext: c = (g^m * r^n) mod n^2
    ll c1 = modExp(g, m, n_sq);
    ll c2 = modExp(r, n, n_sq);
    ll ciphertext = (c1 * c2) % n_sq;
    cout << "Ciphertext: " << ciphertext << endl;

    // -------------------------------
    // Decryption
    // -------------------------------
    ll c_lambda = modExp(ciphertext, lambda, n_sq);
    ll L_c = L_function(c_lambda, n);
    ll decrypted = (L_c * mu) % n;
    cout << "Decrypted plaintext: " << decrypted << endl;

    return 0;
}
