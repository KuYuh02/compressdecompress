#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;
    Node(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

std::unordered_map<char, std::string> generateHuffmanCodes(Node* root) {
    std::unordered_map<char, std::string> huffmanCodes;
    std::function<void(Node*, std::string)> traverse = [&](Node* node, std::string code) {
        if (!node) return;
        if (!node->left && !node->right) {
            huffmanCodes[node->data] = code;
        }
        traverse(node->left, code + "0");
        traverse(node->right, code + "1");
    };
    traverse(root, "");
    return huffmanCodes;
}

Node* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (const auto& pair : freqMap) {
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

std::string compress(const std::string& source) {
    if (source.empty()) return "";
    
    std::unordered_map<char, int> freqMap;
    for (char c : source) {
        freqMap[c]++;
    }
    Node* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffmanCodes = generateHuffmanCodes(root);
    
    std::ostringstream freqStream;
    freqStream.put(static_cast<char>(freqMap.size()));
    for (const auto& pair : freqMap) {
        freqStream.put(pair.first);
        freqStream.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }
    
    std::string bitString;
    for (char c : source) {
        bitString += huffmanCodes[c];
    }
    
    int padding = (8 - (bitString.size() % 8)) % 8;
    bitString.append(padding, '0');
    
    std::ostringstream bitStream;
    bitStream.put(static_cast<char>(padding));
    for (size_t i = 0; i < bitString.size(); i += 8) {
        std::bitset<8> bits(bitString.substr(i, 8));
        bitStream.put(static_cast<char>(bits.to_ulong()));
    }
    
    delete root;
    return freqStream.str() + bitStream.str();
}

std::string decompress(const std::string& source) {
    if (source.empty()) return "";
    
    std::istringstream freqStream(source);
    int numEntries = freqStream.get();
    
    std::unordered_map<char, int> freqMap;
    for (int i = 0; i < numEntries; i++) {
        char ch = freqStream.get();
        int freq;
        freqStream.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[ch] = freq;
    }
    
    Node* root = buildHuffmanTree(freqMap);
    int padding = freqStream.get();
    
    std::string bitString;
    char ch;
    while (freqStream.get(ch)) {
        bitString += std::bitset<8>(static_cast<unsigned char>(ch)).to_string();
    }
    
    bitString = bitString.substr(0, bitString.size() - padding);
    
    std::string decoded;
    Node* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->data;
            current = root;
        }
    }
    
    delete root;
    return decoded;
}
