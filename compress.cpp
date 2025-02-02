#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <sstream>

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

std::string compress(const std::string& source) {
    std::unordered_map<char, int> freqMap;
    for (char ch : source) {
        freqMap[ch]++;
    }
    
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    for (const auto& pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }
    
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();
        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }
    
    Node* root = minHeap.top();
    std::unordered_map<char, std::string> huffmanCodes;
    std::function<void(Node*, std::string)> encode = [&](Node* node, std::string str) {
        if (!node) return;
        if (node->ch != '\0') {
            huffmanCodes[node->ch] = str;
        }
        encode(node->left, str + "0");
        encode(node->right, str + "1");
    };
    encode(root, "");
    
    std::ostringstream header;
    header << huffmanCodes.size() << " ";
    for (const auto& pair : huffmanCodes) {
        header << pair.first << " " << pair.second << " ";
    }
    
    std::ostringstream encodedString;
    for (char ch : source) {
        encodedString << huffmanCodes[ch];
    }
    
    return header.str() + "#" + encodedString.str();
}

std::string decompress(const std::string& source) {
    size_t pos = source.find('#');
    if (pos == std::string::npos) return "";
    
    std::istringstream headerStream(source.substr(0, pos));
    std::string encodedData = source.substr(pos + 1);
    
    std::unordered_map<std::string, char> reverseHuffmanCodes;
    size_t tableSize;
    headerStream >> tableSize;
    for (size_t i = 0; i < tableSize; i++) {
        char ch;
        std::string code;
        headerStream >> ch >> code;
        reverseHuffmanCodes[code] = ch;
    }
    
    std::string decoded, current;
    for (char bit : encodedData) {
        current += bit;
        if (reverseHuffmanCodes.count(current)) {
            decoded += reverseHuffmanCodes[current];
            current.clear();
        }
    }
    
    return decoded;
}
