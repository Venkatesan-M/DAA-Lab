#include <iostream>
#include <vector>

using namespace std;

int main(){

    vector<int> s = {1,2,3,4,5,6,7,8,9,0};

    vector<int>::iterator i;

    for(i = s.begin(); i != s.end(); i++){
        cout << *i <<endl;
    }

    return 0;
}