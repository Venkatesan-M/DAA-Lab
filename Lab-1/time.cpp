#include <iostream>
#include <vector>
#include <ctime>
#include <limits>
#include <string>
#include <algorithm>

using namespace std;

struct Participant {
    string name;
    int score;
};

void measureExecutionTime(int N) {
    vector<Participant> participants(N);

    // Generate random data
    for (int i = 0; i < N; ++i) {
        participants[i].name = "Participant" + to_string(i + 1);
        participants[i].score = rand() % 101; // Random score between 0 and 100
    }

    // Timing the execution
    clock_t start = clock();

    auto minMaxParticipant = minmax_element(
        participants.begin(), participants.end(),
        [](const Participant &p1, const Participant &p2) {
            return p1.score < p2.score;
        });

    Participant minParticipant = *minMaxParticipant.first;
    Participant maxParticipant = *minMaxParticipant.second;

    // Calculate execution time
    double duration = (clock() - start) / (double)CLOCKS_PER_SEC;

    cout << "Number of Participants: " << N << "\n";
    cout << "Time taken to execute: " << duration << " seconds\n";
    cout << "Minimum mark participant: " << minParticipant.name << " with score " << minParticipant.score << "\n";
    cout << "Maximum mark participant: " << maxParticipant.name << " with score " << maxParticipant.score << "\n";
    cout << "-----------------------------------------\n";
}

int main() {
    srand(time(0)); // Seed for random number generation

    // List of participant counts to test
    vector<int> participantCounts = {10, 100, 1000, 10000, 100000};

    for (int count : participantCounts) {
        measureExecutionTime(count);
    }

    return 0;
}
