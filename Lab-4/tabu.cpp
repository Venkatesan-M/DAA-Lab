#include <iostream>
#include <climits>
#include <iomanip>
using namespace std;

int rodcut(int n, int val[]) {
    int res[n + 1];
    res[0] = 0;
    
    for (int i = 1; i <= n; i++) {
        res[i] = INT_MIN;
        for (int j = 0; j < i; j++) {
            res[i] = max(res[i], val[j] + res[i - (j + 1)]);
        }
    }
    return res[n];
}

int main() {
    int n = 5;
    int ar[] = {2, 5, 7, 8, 10};
    cout << "Maximum Revenue: " << rodcut(n, ar) << endl;
    return 0;
}
