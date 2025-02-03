#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
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

void buildHuffmanTree(const std::string& text, std::unordered_map<char, std::string>& huffCodes, std::string& encodedTree) {
    std::unordered_map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }

    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (auto& pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    Node* root = pq.top();
    
    std::function<void(Node*, std::string)> generateCodes = [&](Node* node, std::string code) {
        if (!node) return;
        if (node->ch != '\0') {
            huffCodes[node->ch] = code;
            encodedTree += node->ch + code + " ";
        }
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    };
    
    generateCodes(root, "");
}

std::string compress(const std::string& source) {
    std::unordered_map<char, std::string> huffCodes;
    std::string encodedTree;
    buildHuffmanTree(source, huffCodes, encodedTree);
    
    std::string compressedData;
    for (char c : source) {
        compressedData += huffCodes[c];
    }
    
    std::ostringstream oss;
    oss << encodedTree << "|";
    for (size_t i = 0; i < compressedData.size(); i += 8) {
        std::bitset<8> bits(compressedData.substr(i, 8));
        oss << static_cast<char>(bits.to_ulong());
    }
    
    return oss.str();
}

std::string decompress(const std::string& source) {
    size_t delim = source.find('|');
    std::unordered_map<std::string, char> decodeMap;
    
    std::istringstream iss(source.substr(0, delim));
    std::string token;
    while (iss >> token) {
        char ch = token[0];
        std::string code = token.substr(1);
        decodeMap[code] = ch;
    }
    
    std::string bitString;
    for (size_t i = delim + 1; i < source.size(); ++i) {
        std::bitset<8> bits(static_cast<unsigned char>(source[i]));
        bitString += bits.to_string();
    }
    
    std::string decoded;
    std::string current;
    for (char bit : bitString) {
        current += bit;
        if (decodeMap.count(current)) {
            decoded += decodeMap[current];
            current.clear();
        }
    }
    
    return decoded;
}
