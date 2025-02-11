#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <vector>
#include <cstdint>

struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int freq) : character(c), frequency(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

void buildTreeStructure(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        structure += '1';
        structure += node->character;
    } else {
        structure += '0';
        buildTreeStructure(node->right, structure);
        buildTreeStructure(node->left, structure);
    }
}

HuffmanNode* reconstructTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;

    if (data[pos] == '1') {
        pos++;
        HuffmanNode* leaf = new HuffmanNode(data[pos], 0);
        pos++;
        return leaf;
    }

    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->right = reconstructTree(data, pos);
    node->left = reconstructTree(data, pos);
    return node;
}

HuffmanNode* constructHuffmanTree(std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> heap;
    
    for (const auto& pair : frequencies) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (heap.size() > 1) {
        HuffmanNode* right = heap.top(); heap.pop();
        HuffmanNode* left = heap.top(); heap.pop();

        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->right = right;
        parent->left = left;
        heap.push(parent);
    }
    
    return heap.top();
}

void generateHuffmanCodes(HuffmanNode* node, const std::string& path, std::map<char, std::string>& codes) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        codes[node->character] = path;
    }
    
    generateHuffmanCodes(node->right, path + "0", codes);
    generateHuffmanCodes(node->left, path + "1", codes);
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->right);
    freeTree(node->left);
    delete node;
}

std::string decompress(const std::string& data) {
    if (data.size() < 8) return "";
    
    const unsigned char* rawData = reinterpret_cast<const unsigned char*>(data.data());
    uint32_t treeSize = (rawData[0] << 24) | (rawData[1] << 16) | (rawData[2] << 8) | rawData[3];
    uint32_t bitLength = (rawData[4] << 24) | (rawData[5] << 16) | (rawData[6] << 8) | rawData[7];

    size_t treeStart = 8;
    size_t treeEnd = treeStart + treeSize;
    if (data.size() < treeEnd) return "";

    std::string treeData = data.substr(treeStart, treeSize);
    std::string encodedData = data.substr(treeEnd);
    
    size_t pos = 0;
    HuffmanNode* root = reconstructTree(treeData, pos);
    
    std::string bitString;
    for (char byte : encodedData) {
        std::bitset<8> bitset(static_cast<unsigned char>(byte));
        bitString += bitset.to_string();
    }
    if (bitString.size() > bitLength) {
        bitString.resize(bitLength);
    }
    
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->right : current->left;
        if (!current->left && !current->right) {
            output += current->character;
            current = root;
        }
    }

    freeTree(root);
    return output;
}
