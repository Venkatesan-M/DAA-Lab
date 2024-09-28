#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;

int N;  // N will be passed as a variable for flexibility in board size

void printSolution(vector<vector<int>>& board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << (board[i][j] ? "Q " : ". ");
        }
        cout << endl;
    }
    cout << endl;
}

bool isSafe(vector<vector<int>>& board, int row, int col) {
    for (int i = 0; i < col; i++)
        if (board[row][i])
            return false;

    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
        if (board[i][j])
            return false;

    for (int i = row, j = col; i < N && j >= 0; i++, j--)
        if (board[i][j])
            return false;

    return true;
}

bool solveNQUtil(vector<vector<int>>& board, int col) {
    if (col >= N) {
        // printSolution(board);  // Uncomment if you want to print solutions
        return true;
    }

    bool res = false;
    for (int i = 0; i < N; i++) {
        if (isSafe(board, i, col)) {
            board[i][col] = 1;
            res = solveNQUtil(board, col + 1) || res;
            board[i][col] = 0;
        }
    }

    return res;
}

void solveNQ() {
    vector<vector<int>> board(N, vector<int>(N, 0));
    solveNQUtil(board, 0);
}

int main() {
    freopen("nqueen_times.txt", "w", stdout);  // Redirect output to a file
    
    vector<int> sizes = {4, 8, 10, 12, 14};  // Different board sizes for N-Queen

    for (int size : sizes) {
        N = size;

        auto start = chrono::high_resolution_clock::now();
        solveNQ();
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "N=" << N << ": " << duration.count() << " microseconds" << endl;
    }

    return 0;
}
