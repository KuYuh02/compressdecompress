#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char symbol;
    int weight;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char s, int w) : symbol(s), weight(w), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->weight > b->weight;
    }
};

HuffmanNode* constructTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* combined = new HuffmanNode('\0', left->weight + right->weight);
        combined->left = left;
        combined->right = right;
        pq.push(combined);
    }
    return pq.top();
}

void generateCodes(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& encodingMap) {
    if (!node) return;
    if (!node->left && !node->right) {
        encodingMap[node->symbol] = path;
    }
    generateCodes(node->left, path + "0", encodingMap);
    generateCodes(node->right, path + "1", encodingMap);
}

void serializeTree(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    structure += node->left || node->right ? "0" : "1";
    if (!node->left && !node->right) {
        structure += node->symbol;
    } else {
        serializeTree(node->left, structure);
        serializeTree(node->right, structure);
    }
}

HuffmanNode* deserializeTree(const std::string& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    if (data[index++] == '1') {
        node->symbol = data[index++];
    } else {
        node->left = deserializeTree(data, index);
        node->right = deserializeTree(data, index);
    }
    return node;
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::unordered_map<char, int> freqTable;
    for (char ch : input) freqTable[ch]++;
    HuffmanNode* root = constructTree(freqTable);
    std::unordered_map<char, std::string> codeMap;
    generateCodes(root, "", codeMap);
    std::string bitStream;
    for (char ch : input) bitStream += codeMap[ch];
    std::string serializedTree;
    serializeTree(root, serializedTree);
    uint32_t treeSize = serializedTree.size(), bitCount = bitStream.size();
    std::string compressedData = std::bitset<32>(bitCount).to_string() + bitStream;
    freeTree(root);
    return std::bitset<32>(treeSize).to_string() + serializedTree + compressedData;
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 64) return "";
    size_t pos = 0;
    uint32_t treeSize = std::bitset<32>(compressed.substr(pos, 32)).to_ulong(); pos += 32;
    uint32_t bitCount = std::bitset<32>(compressed.substr(pos, 32)).to_ulong(); pos += 32;
    HuffmanNode* root = deserializeTree(compressed.substr(pos, treeSize), pos);
    std::string bitStream = compressed.substr(pos, bitCount);
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitStream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->symbol;
            current = root;
        }
    }
    freeTree(root);
    return output;
}
