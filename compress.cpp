#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <vector>
#include <cstdint>

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
    treeEncoding += root->left || root->right ? '0' : '1';
    if (!root->left && !root->right) {
        treeEncoding += root->data;
    } else {
        encodeTree(root->left, treeEncoding);
        encodeTree(root->right, treeEncoding);
    }
}

HuffmanNode* decodeTree(const std::string& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    if (data[index++] == '1') {
        node->data = data[index++];
    } else {
        node->left = decodeTree(data, index);
        node->right = decodeTree(data, index);
    }
    return node;
}

HuffmanNode* buildHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;
    for (const auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}

void generateHuffmanCodes(HuffmanNode* root, const std::string& path, std::map<char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->data] = path;
    }
    generateHuffmanCodes(root->left, path + "0", codes);
    generateHuffmanCodes(root->right, path + "1", codes);
}

void freeTree(HuffmanNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::map<char, int> freqMap;
    for (char c : input) {
        freqMap[c]++;
    }
    HuffmanNode* treeRoot = buildHuffmanTree(freqMap);
    std::map<char, std::string> huffmanCodes;
    generateHuffmanCodes(treeRoot, "", huffmanCodes);
    std::string encodedData;
    for (char c : input) {
        encodedData += huffmanCodes[c];
    }
    std::string treeEncoding;
    encodeTree(treeRoot, treeEncoding);
    uint32_t treeSize = treeEncoding.size();
    uint32_t bitSize = encodedData.size();
    std::string binaryData;
    for (size_t i = 0; i < encodedData.size(); i += 8) {
        std::bitset<8> byte(encodedData.substr(i, 8));
        binaryData += static_cast<char>(byte.to_ulong());
    }
    std::string header(8, '\0');
    for (int i = 0; i < 4; i++) {
        header[i] = (treeSize >> (24 - i * 8)) & 0xFF;
        header[i + 4] = (bitSize >> (24 - i * 8)) & 0xFF;
    }
    freeTree(treeRoot);
    return header + treeEncoding + binaryData;
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 8) return "";
    size_t index = 0;
    uint32_t treeSize = (compressed[index++] << 24) | (compressed[index++] << 16) | (compressed[index++] << 8) | compressed[index++];
    uint32_t bitSize = (compressed[index++] << 24) | (compressed[index++] << 16) | (compressed[index++] << 8) | compressed[index++];
    if (compressed.size() < index + treeSize) return "";
    std::string treeEncoding = compressed.substr(index, treeSize);
    std::string binaryData = compressed.substr(index + treeSize);
    index = 0;
    HuffmanNode* root = decodeTree(treeEncoding, index);
    std::string bitString;
    for (char c : binaryData) {
        bitString += std::bitset<8>(static_cast<unsigned char>(c)).to_string();
    }
    bitString.resize(bitSize);
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->data;
            current = root;
        }
    }
    freeTree(root);
    return output;
}
