#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

    HuffmanNode(char c, int f) : character(c), frequency(f), leftChild(nullptr), rightChild(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

void serializeHuffmanTree(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    
    if (!node->leftChild && !node->rightChild) {
        structure += "1";
        structure += node->character;
    } else {
        structure += "0";
        serializeHuffmanTree(node->leftChild, structure);
        serializeHuffmanTree(node->rightChild, structure);
    }
}

HuffmanNode* deserializeHuffmanTree(const std::string& data, size_t& idx) {
    if (idx >= data.size()) return nullptr;

    if (data[idx] == '1') {
        idx++;
        HuffmanNode* leaf = new HuffmanNode(data[idx], 0);
        idx++;
        return leaf;
    }

    idx++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeHuffmanTree(data, idx);
    node->rightChild = deserializeHuffmanTree(data, idx);
    return node;
}

HuffmanNode* buildHuffmanTree(std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;

    for (const auto& entry : frequencies) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* merged = new HuffmanNode('\0', left->frequency + right->frequency);
        merged->leftChild = left;
        merged->rightChild = right;
        minHeap.push(merged);
    }
    return minHeap.top();
}

void generateHuffmanCodes(HuffmanNode* root, const std::string& code, std::map<char, std::string>& codeMap) {
    if (!root) return;
    if (!root->leftChild && !root->rightChild) {
        codeMap[root->character] = code;
    }
    generateHuffmanCodes(root->leftChild, code + "0", codeMap);
    generateHuffmanCodes(root->rightChild, code + "1", codeMap);
}

void freeHuffmanTree(HuffmanNode* root) {
    if (!root) return;
    freeHuffmanTree(root->leftChild);
    freeHuffmanTree(root->rightChild);
    delete root;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    
    std::map<char, int> frequencyTable;
    for (char c : input) frequencyTable[c]++;
    
    HuffmanNode* treeRoot = buildHuffmanTree(frequencyTable);
    std::map<char, std::string> huffmanCodes;
    generateHuffmanCodes(treeRoot, "", huffmanCodes);

    std::string encodedString;
    for (char c : input) encodedString += huffmanCodes[c];
    
    std::string serializedTree;
    serializeHuffmanTree(treeRoot, serializedTree);
    uint32_t treeSize = serializedTree.size();
    uint32_t bitSize = encodedString.size();
    
    std::string compressedData = encodedString;
    std::string header(8, '\0');
    header[0] = (treeSize >> 24) & 0xFF;
    header[1] = (treeSize >> 16) & 0xFF;
    header[2] = (treeSize >> 8) & 0xFF;
    header[3] = treeSize & 0xFF;
    header[4] = (bitSize >> 24) & 0xFF;
    header[5] = (bitSize >> 16) & 0xFF;
    header[6] = (bitSize >> 8) & 0xFF;
    header[7] = bitSize & 0xFF;
    
    freeHuffmanTree(treeRoot);
    return header + serializedTree + compressedData;
}

std::string decompress(const std::string& input) {
    if (input.size() < 8) return "";
    
    const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
    uint32_t treeSize = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    uint32_t bitSize = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    
    size_t treeDataEnd = 8 + treeSize;
    if (input.size() < treeDataEnd) return "";
    
    std::string treeData = input.substr(8, treeSize);
    std::string binaryData = input.substr(treeDataEnd);
    
    size_t index = 0;
    HuffmanNode* treeRoot = deserializeHuffmanTree(treeData, index);
    
    std::string output;
    HuffmanNode* current = treeRoot;
    for (char bit : binaryData) {
        current = (bit == '0') ? current->leftChild : current->rightChild;
        if (!current->leftChild && !current->rightChild) {
            output += current->character;
            current = treeRoot;
        }
    }
    
    freeHuffmanTree(treeRoot);
    return output;
}
