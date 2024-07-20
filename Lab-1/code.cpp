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

int main() {
    int N;
    cout << "Enter the number of participants: ";
    cin >> N;

    vector<Participant> participants(N);

    for (int i = 0; i < N; ++i) {
        cout << "Enter name and score for participant " << i + 1 << ": ";
        cin >> participants[i].name >> participants[i].score;
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

    cout << "Time taken to execute: " << duration << " seconds\n";
    cout << "Minimum mark participant: " << minParticipant.name << " with score " << minParticipant.score << "\n";
    cout << "Maximum mark participant: " << maxParticipant.name << " with score " << maxParticipant.score << "\n";

    return 0;
}
