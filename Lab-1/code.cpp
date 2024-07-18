#include <iostream>
#include <vector>

using namespace std;

int main(){
    vector<int> marks;
    for (int i = 1; i <= 5; i++){
        cout << "Enter the mark of student"<< endl;
        int x;
        cin >> x;
        marks.push_back(x);
    }

    auto min = marks.begin(); 
    auto max = marks.begin();

    for (int i = marks.at(1); i != *marks.end(); ++i)
        cout << i << " ";

    return 0;
}