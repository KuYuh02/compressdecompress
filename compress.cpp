#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char symbol;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char s, int f) : symbol(s), frequency(f), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

HuffmanNode* createHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;
    for (const auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* merged = new HuffmanNode('\0', left->frequency + right->frequency);
        merged->left = left;
        merged->right = right;
        minHeap.push(merged);
    }
    return minHeap.top();
}

void generateCodes(HuffmanNode* node, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        codes[node->symbol] = code;
    }
    generateCodes(node->left, code + "0", codes);
    generateCodes(node->right, code + "1", codes);
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

HuffmanNode* deserializeTree(const std::string& serialized, size_t& pos) {
    if (pos >= serialized.size()) return nullptr;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    if (serialized[pos++] == '1') {
        node->symbol = serialized[pos++];
    } else {
        node->left = deserializeTree(serialized, pos);
        node->right = deserializeTree(serialized, pos);
    }
    return node;
}

void deleteTree(HuffmanNode* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::unordered_map<char, int> frequencies;
    for (char ch : input) frequencies[ch]++;
    HuffmanNode* root = createHuffmanTree(frequencies);
    std::unordered_map<char, std::string> encodingMap;
    generateCodes(root, "", encodingMap);
    std::string bitString;
    for (char ch : input) bitString += encodingMap[ch];
    std::string treeRepresentation;
    serializeTree(root, treeRepresentation);
    uint32_t treeSize = treeRepresentation.size(), bitSize = bitString.size();
    std::string binaryData = std::bitset<32>(bitSize).to_string() + bitString;
    deleteTree(root);
    return std::bitset<32>(treeSize).to_string() + treeRepresentation + binaryData;
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 64) return "";
    size_t position = 0;
    uint32_t treeSize = std::bitset<32>(compressed.substr(position, 32)).to_ulong(); position += 32;
    uint32_t bitCount = std::bitset<32>(compressed.substr(position, 32)).to_ulong(); position += 32;
    HuffmanNode* root = deserializeTree(compressed.substr(position, treeSize), position);
    std::string bitSequence = compressed.substr(position, bitCount);
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitSequence) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->symbol;
            current = root;
        }
    }
    deleteTree(root);
    return output;
}
