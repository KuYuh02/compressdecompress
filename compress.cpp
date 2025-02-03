#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>
#include <fstream>

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

void buildHuffmanTree(const std::string& text, std::unordered_map<char, std::string>& huffCodes, Node*& root, std::unordered_map<char, int>& freqMap) {
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
    root = pq.top();
    
    std::function<void(Node*, std::string)> generateCodes = [&](Node* node, std::string code) {
        if (!node) return;
        if (node->ch != '\0') {
            huffCodes[node->ch] = code;
        }
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    };
    
    generateCodes(root, "");
}

std::string compress(const std::string& source) {
    std::unordered_map<char, int> freqMap;
    for (char c : source) {
        freqMap[c]++;
    }
    
    std::unordered_map<char, std::string> huffCodes;
    Node* root = nullptr;
    buildHuffmanTree(source, huffCodes, root, freqMap);
    
    std::ostringstream freqStream;
    freqStream << freqMap.size() << " ";
    for (auto& pair : freqMap) {
        freqStream << pair.first << pair.second << " ";
    }
    
    std::string compressedData;
    for (char c : source) {
        compressedData += huffCodes[c];
    }
    
    std::ostringstream bitStream;
    for (size_t i = 0; i < compressedData.size(); i += 8) {
        std::bitset<8> bits(compressedData.substr(i, 8));
        bitStream << static_cast<char>(bits.to_ulong());
    }
    
    return freqStream.str() + "|" + bitStream.str();
}

Node* rebuildHuffmanTree(std::unordered_map<char, int>& freqMap) {
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
    return pq.top();
}

std::string decompress(const std::string& source) {
    size_t delim = source.find('|');
    std::istringstream freqStream(source.substr(0, delim));
    int numEntries;
    freqStream >> numEntries;
    
    std::unordered_map<char, int> freqMap;
    for (int i = 0; i < numEntries; i++) {
        char ch;
        int freq;
        freqStream >> ch >> freq;
        freqMap[ch] = freq;
    }
    
    Node* root = rebuildHuffmanTree(freqMap);
    
    std::string bitString;
    for (size_t i = delim + 1; i < source.size(); ++i) {
        std::bitset<8> bits(static_cast<unsigned char>(source[i]));
        bitString += bits.to_string();
    }
    
    std::string decoded;
    Node* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    
    return decoded;
}
