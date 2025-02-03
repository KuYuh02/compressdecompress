#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>
#include <memory>

struct Node {
    char ch;
    int freq;
    std::shared_ptr<Node> left, right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
        return a->freq > b->freq;
    }
};

void buildHuffmanTree(const std::string& text, std::unordered_map<char, std::string>& huffCodes, std::shared_ptr<Node>& root) {
    std::unordered_map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }

    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Compare> pq;
    for (auto& pair : freqMap) {
        pq.push(std::make_shared<Node>(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        auto parent = std::make_shared<Node>('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    root = pq.top();
    
    std::function<void(std::shared_ptr<Node>, std::string)> generateCodes = [&](std::shared_ptr<Node> node, std::string code) {
        if (!node) return;
        if (node->ch != '\0') {
            huffCodes[node->ch] = code;
        }
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    };
    
    generateCodes(root, "");
}

void serializeTree(std::shared_ptr<Node> root, std::string& encodedTree) {
    if (!root) {
        encodedTree += "0";
        return;
    }
    encodedTree += "1";
    encodedTree += root->ch;
    serializeTree(root->left, encodedTree);
    serializeTree(root->right, encodedTree);
}

std::string compress(const std::string& source) {
    std::unordered_map<char, std::string> huffCodes;
    std::shared_ptr<Node> root = nullptr;
    buildHuffmanTree(source, huffCodes, root);
    
    std::string encodedTree;
    serializeTree(root, encodedTree);
    
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

std::shared_ptr<Node> deserializeTree(std::istringstream& iss) {
    char marker;
    iss.get(marker);
    if (marker == '0') return nullptr;
    char ch;
    iss.get(ch);
    auto node = std::make_shared<Node>(ch, 0);
    node->left = deserializeTree(iss);
    node->right = deserializeTree(iss);
    return node;
}

std::string decompress(const std::string& source) {
    size_t delim = source.find('|');
    std::istringstream iss(source.substr(0, delim));
    std::shared_ptr<Node> root = deserializeTree(iss);
    
    std::string bitString;
    for (size_t i = delim + 1; i < source.size(); ++i) {
        std::bitset<8> bits(static_cast<unsigned char>(source[i]));
        bitString += bits.to_string();
    }
    
    std::string decoded;
    auto current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    
    return decoded;
}
