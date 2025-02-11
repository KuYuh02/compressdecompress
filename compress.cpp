#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char character;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char ch, int f) : character(ch), freq(f), left(nullptr), right(nullptr) {}
};

struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* combined = new HuffmanNode('\0', left->freq + right->freq);
        combined->left = left;
        combined->right = right;
        pq.push(combined);
    }
    return pq.top();
}

void generateHuffmanCodes(HuffmanNode* node, const std::string& prefix, std::unordered_map<char, std::string>& codeMap) {
    if (!node) return;
    if (!node->left && !node->right) {
        codeMap[node->character] = prefix;
    }
    generateHuffmanCodes(node->left, prefix + "0", codeMap);
    generateHuffmanCodes(node->right, prefix + "1", codeMap);
}

void encodeHuffmanTree(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    structure += node->left || node->right ? "0" : "1";
    if (!node->left && !node->right) {
        structure += node->character;
    } else {
        encodeHuffmanTree(node->left, structure);
        encodeHuffmanTree(node->right, structure);
    }
}

HuffmanNode* decodeHuffmanTree(const std::string& serialized, size_t& index) {
    if (index >= serialized.size()) return nullptr;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    if (serialized[index++] == '1') {
        node->character = serialized[index++];
    } else {
        node->left = decodeHuffmanTree(serialized, index);
        node->right = decodeHuffmanTree(serialized, index);
    }
    return node;
}

void freeHuffmanTree(HuffmanNode* node) {
    if (!node) return;
    freeHuffmanTree(node->left);
    freeHuffmanTree(node->right);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::unordered_map<char, int> freqMap;
    for (char ch : input) freqMap[ch]++;
    HuffmanNode* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    std::string encodedBits;
    for (char ch : input) encodedBits += huffmanCodes[ch];
    std::string treeStructure;
    encodeHuffmanTree(root, treeStructure);
    uint32_t treeSize = treeStructure.size(), bitSize = encodedBits.size();
    std::string binaryData = std::bitset<32>(bitSize).to_string() + encodedBits;
    freeHuffmanTree(root);
    return std::bitset<32>(treeSize).to_string() + treeStructure + binaryData;
}

std::string decompress(const std::string& compressedData) {
    if (compressedData.size() < 64) return "";
    size_t pos = 0;
    uint32_t treeSize = std::bitset<32>(compressedData.substr(pos, 32)).to_ulong(); pos += 32;
    uint32_t bitCount = std::bitset<32>(compressedData.substr(pos, 32)).to_ulong(); pos += 32;
    HuffmanNode* root = decodeHuffmanTree(compressedData.substr(pos, treeSize), pos);
    std::string bitStream = compressedData.substr(pos, bitCount);
    std::string decodedText;
    HuffmanNode* current = root;
    for (char bit : bitStream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decodedText += current->character;
            current = root;
        }
    }
    freeHuffmanTree(root);
    return decodedText;
}
