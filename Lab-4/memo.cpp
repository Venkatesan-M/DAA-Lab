#include <iostream>
#include <climits>
#include <iomanip>
using namespace std;

int rodhelp(int n, int val[], int re[]) {
    if (n == 0) {
        return 0;
    }
    if (re[n] != -1) {
        return re[n];
    }
    int maxre = INT_MIN;
    for (int i = 0; i < n; i++) {
        maxre = max(maxre, val[i] + rodhelp(n - (i + 1), val, re));
    }
    re[n] = maxre;
    return re[n];
}

int rodcut(int n, int val[]) {
    int re[n + 1];
    for (int i = 0; i <= n; i++) {
        re[i] = -1;
    }
    return rodhelp(n, val, re);
}

int main() {
    int n = 5;
    int ar[] = {2, 5, 7, 8, 10};
    cout << "Maximum Revenue: " << rodcut(n, ar) << endl;
    return 0;
}
