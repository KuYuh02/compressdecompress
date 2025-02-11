#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <vector>
#include <cstdint>
#include <cstring>

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void encodeTree(HuffmanNode* root, std::string& treeEncoding) {
    if (!root) return;
    treeEncoding += (root->left || root->right) ? '0' : '1';
    if (!root->left && !root->right) {
        treeEncoding += root->data;
    } else {
        encodeTree(root->left, treeEncoding);
        encodeTree(root->right, treeEncoding);
    }
}

HuffmanNode* decodeTree(const std::string& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    if (data[index++] == '1') {
        return new HuffmanNode(data[index++], 0);
    }
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(data, index);
    node->right = decodeTree(data, index);
    return node;
}

HuffmanNode* buildHuffmanTree(std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;
    for (const auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}

void generateHuffmanCodes(HuffmanNode* root, const std::string& path, std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->data] = path;
    }
    generateHuffmanCodes(root->left, path + "1", codes);
    generateHuffmanCodes(root->right, path + "0", codes);
}

void freeTree(HuffmanNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::unordered_map<char, int> freqMap;
    for (char c : input) {
        freqMap[c]++;
    }
    HuffmanNode* treeRoot = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(treeRoot, "", huffmanCodes);
    std::string encodedData;
    for (char c : input) {
        encodedData += huffmanCodes[c];
    }
    std::string treeEncoding;
    encodeTree(treeRoot, treeEncoding);
    uint32_t treeSize = treeEncoding.size();
    uint32_t bitSize = encodedData.size();
    std::string binaryData((bitSize + 7) / 8, 0);
    for (size_t i = 0; i < bitSize; i++) {
        if (encodedData[i] == '1') {
            binaryData[i / 8] |= (1 << (5 - (i % 8)));
        }
    }
    std::string header(8, 0);
    std::memcpy(&header[0], &treeSize, sizeof(treeSize));
    std::memcpy(&header[4], &bitSize, sizeof(bitSize));
    freeTree(treeRoot);
    return header + treeEncoding + binaryData;
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 8) return "";
    size_t index = 0;
    uint32_t treeSize, bitSize;
    std::memcpy(&treeSize, &compressed[index], sizeof(treeSize));
    index += sizeof(treeSize);
    std::memcpy(&bitSize, &compressed[index], sizeof(bitSize));
    index += sizeof(bitSize);
    if (compressed.size() < index + treeSize) return "";
    std::string treeEncoding = compressed.substr(index, treeSize);
    std::string binaryData = compressed.substr(index + treeSize);
    index = 0;
    HuffmanNode* root = decodeTree(treeEncoding, index);
    std::string bitString;
    for (size_t i = 0; i < bitSize; i++) {
        bitString += (binaryData[i / 8] & (1 << (5 - (i % 8)))) ? '1' : '0';
    }
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '1') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->data;
            current = root;
        }
    }
    freeTree(root);
    return output;
}
