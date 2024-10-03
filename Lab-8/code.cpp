#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Node structure for Huffman Tree
struct Node {
    char ch;
    float freq;
    Node *left, *right;
    
    Node(char ch, float freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
};

// Custom comparator for priority queue
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Recursively build and assign codes
void assignCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCode[root->ch] = code;
    
    assignCodes(root->left, code + "0", huffmanCode);
    assignCodes(root->right, code + "1", huffmanCode);
}

// Huffman encoding algorithm
void huffmanEncoding(vector<char>& letters, vector<float>& frequencies) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    for (int i = 0; i < letters.size(); i++) {
        pq.push(new Node(letters[i], frequencies[i]));
    }
    
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        
        float sum = left->freq + right->freq;
        Node* newNode = new Node('\0', sum);
        newNode->left = left;
        newNode->right = right;
        
        pq.push(newNode);
    }
    
    Node* root = pq.top();
    unordered_map<char, string> huffmanCode;
    assignCodes(root, "", huffmanCode);
    
    cout << "Huffman Codes:\n";
    for (auto pair : huffmanCode) {
        cout << pair.first << ": " << pair.second << "\n";
    }
    
    // Calculate expected encoding length
    float expectedLength = 0;
    for (int i = 0; i < letters.size(); i++) {
        expectedLength += frequencies[i] * huffmanCode[letters[i]].length();
    }
    
    cout << "Expected Encoding Length: " << expectedLength << "\n";
}

int main() {
    vector<char> letters = {'a', 'g', 'h', 'i', 'l', 'm', 'o', 'r', 't'};
    vector<float> frequencies = {0.14, 0.06, 0.10, 0.17, 0.07, 0.11, 0.10, 0.08, 0.17};
    
    huffmanEncoding(letters, frequencies);
    return 0;
}
