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

void buildTreeString(HuffmanNode* root, std::string& treeStr) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        treeStr += "1";
        treeStr += root->data;
    } else {
        treeStr += "0";
        buildTreeString(root->left, treeStr);
        buildTreeString(root->right, treeStr);
    }
}

HuffmanNode* rebuildTree(const std::string& treeData, size_t& idx) {
    if (idx >= treeData.size()) return nullptr;

    if (treeData[idx] == '1') {
        idx++;
        HuffmanNode* leaf = new HuffmanNode(treeData[idx], 0);
        idx++;
        return leaf;
    }

    idx++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = rebuildTree(treeData, idx);
    node->right = rebuildTree(treeData, idx);
    return node;
}

HuffmanNode* generateHuffmanTree(std::unordered_map<char, int>& freqMap) {
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

void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;

    if (!root->left && !root->right) {
        codeMap[root->data] = prefix;
    }

    generateCodes(root->left, prefix + "0", codeMap);
    generateCodes(root->right, prefix + "1", codeMap);
}

void freeTree(HuffmanNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

std::string binaryEncode(const std::string& bitStream) {
    std::stringstream out;
    for (size_t i = 0; i < bitStream.size(); i += 8) {
        std::bitset<8> bits(bitStream.substr(i, 8));
        out.put(static_cast<char>(bits.to_ulong()));
    }
    return out.str();
}

std::string binaryDecode(const std::string& binData) {
    std::string bitStream;
    for (char c : binData) {
        std::bitset<8> bits(static_cast<unsigned char>(c));
        bitStream += bits.to_string();
    }
    return bitStream;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";

    std::unordered_map<char, int> freqMap;
    for (char c : input) freqMap[c]++;

    HuffmanNode* root = generateHuffmanTree(freqMap);
    std::unordered_map<char, std::string> codeMap;
    generateCodes(root, "", codeMap);

    std::string encodedBits;
    for (char c : input) {
        encodedBits += codeMap[c];
    }

    std::string treeString;
    buildTreeString(root, treeString);
    uint32_t treeSize = treeString.size();
    uint32_t bitLength = encodedBits.size();
    std::string binaryEncoded = binaryEncode(encodedBits);

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
    return header + treeString + binaryEncoded;
}

std::string decompress(const std::string& compressed) {
    if (compressed.size() < 8) return "";

    const unsigned char* data = reinterpret_cast<const unsigned char*>(compressed.data());
    uint32_t treeSize = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    uint32_t bitLength = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

    size_t headerSize = 8;
    size_t treeEnd = headerSize + treeSize;
    if (compressed.size() < treeEnd) return "";

    std::string treeData = compressed.substr(headerSize, treeSize);
    std::string binaryData = compressed.substr(treeEnd);

    size_t idx = 0;
    HuffmanNode* root = rebuildTree(treeData, idx);

    std::string bitStream = binaryDecode(binaryData);
    if (bitStream.size() > bitLength) bitStream.resize(bitLength);

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
