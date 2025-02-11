#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int f) : data(c), freq(f), left(nullptr), right(nullptr) {}
};

struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void serializeTree(HuffmanNode* root, std::string& treeStr) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        treeStr += "1";
        treeStr += root->data;
    } else {
        treeStr += "0";
        serializeTree(root->left, treeStr);
        serializeTree(root->right, treeStr);
    }
}

HuffmanNode* deserializeTree(const std::string& treeData, size_t& idx) {
    if (idx >= treeData.size()) return nullptr;

    if (treeData[idx] == '1') {
        idx++;
        HuffmanNode* leaf = new HuffmanNode(treeData[idx], 0);
        idx++;
        return leaf;
    }

    idx++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = deserializeTree(treeData, idx);
    node->right = deserializeTree(treeData, idx);
    return node;
}

HuffmanNode* buildHuffmanTree(std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (const auto& pair : freqMap) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
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

void generateHuffmanCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;

    if (!root->left && !root->right) {
        codeMap[root->data] = prefix;
    }

    generateHuffmanCodes(root->left, prefix + "0", codeMap);
    generateHuffmanCodes(root->right, prefix + "1", codeMap);
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
    for (char c : input) freqMap[c]++;

    HuffmanNode* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> codeMap;
    generateHuffmanCodes(root, "", codeMap);

    std::string encodedBits;
    for (char c : input) {
        encodedBits += codeMap[c];
    }

    std::string treeString;
    serializeTree(root, treeString);
    uint32_t treeSize = treeString.size();
    uint32_t bitLength = encodedBits.size();
    
    std::stringstream binaryEncoded;
    for (size_t i = 0; i < encodedBits.size(); i += 8) {
        std::bitset<8> bits(encodedBits.substr(i, 8));
        binaryEncoded.put(static_cast<char>(bits.to_ulong()));
    }

    std::string header(8, '\0');
    header[0] = (treeSize >> 24) & 0xFF;
    header[1] = (treeSize >> 16) & 0xFF;
    header[2] = (treeSize >> 8) & 0xFF;
    header[3] = treeSize & 0xFF;
    header[4] = (bitLength >> 24) & 0xFF;
    header[5] = (bitLength >> 16) & 0xFF;
    header[6] = (bitLength >> 8) & 0xFF;
    header[7] = bitLength & 0xFF;

    freeTree(root);
    return header + treeString + binaryEncoded.str();
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 8) return "";

    size_t idx = 8;
    uint32_t treeSize = (compressed[0] << 24) | (compressed[1] << 16) | (compressed[2] << 8) | compressed[3];
    uint32_t bitLength = (compressed[4] << 24) | (compressed[5] << 16) | (compressed[6] << 8) | compressed[7];

    HuffmanNode* root = deserializeTree(compressed.substr(idx, treeSize), idx);

    std::string bitStream;
    for (size_t i = idx + treeSize; i < compressed.size(); i++) {
        std::bitset<8> bits(static_cast<unsigned char>(compressed[i]));
        bitStream += bits.to_string();
    }
    bitStream.resize(bitLength);

    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitStream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->data;
            current = root;
        }
    }

    freeTree(root);
    return output;
}
