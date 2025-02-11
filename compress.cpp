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
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;
    HuffmanNode(char s, int w) : symbol(s), weight(w), leftChild(nullptr), rightChild(nullptr) {}
};

struct CompareHuffmanNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->weight > b->weight;
    }
};

HuffmanNode* constructHuffmanTree(const std::unordered_map<char, int>& frequencyTable) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareHuffmanNodes> nodeQueue;
    for (const auto& entry : frequencyTable) {
        nodeQueue.push(new HuffmanNode(entry.first, entry.second));
    }
    while (nodeQueue.size() > 1) {
        HuffmanNode* left = nodeQueue.top(); nodeQueue.pop();
        HuffmanNode* right = nodeQueue.top(); nodeQueue.pop();
        HuffmanNode* mergedNode = new HuffmanNode('\0', left->weight + right->weight);
        mergedNode->leftChild = left;
        mergedNode->rightChild = right;
        nodeQueue.push(mergedNode);
    }
    return nodeQueue.top();
}

void createHuffmanEncoding(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& encodingMap) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        encodingMap[node->symbol] = path;
    }
    createHuffmanEncoding(node->leftChild, path + "0", encodingMap);
    createHuffmanEncoding(node->rightChild, path + "1", encodingMap);
}

void serializeHuffmanTree(HuffmanNode* node, std::string& treeData) {
    if (!node) return;
    treeData += node->leftChild || node->rightChild ? "0" : "1";
    if (!node->leftChild && !node->rightChild) {
        treeData += node->symbol;
    } else {
        serializeHuffmanTree(node->leftChild, treeData);
        serializeHuffmanTree(node->rightChild, treeData);
    }
}

HuffmanNode* deserializeHuffmanTree(const std::string& treeData, size_t& index) {
    if (index >= treeData.size()) return nullptr;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    if (treeData[index++] == '1') {
        node->symbol = treeData[index++];
    } else {
        node->leftChild = deserializeHuffmanTree(treeData, index);
        node->rightChild = deserializeHuffmanTree(treeData, index);
    }
    return node;
}

void destroyHuffmanTree(HuffmanNode* node) {
    if (!node) return;
    destroyHuffmanTree(node->leftChild);
    destroyHuffmanTree(node->rightChild);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    std::unordered_map<char, int> frequencyTable;
    for (char ch : input) frequencyTable[ch]++;
    HuffmanNode* root = constructHuffmanTree(frequencyTable);
    std::unordered_map<char, std::string> encodingMap;
    createHuffmanEncoding(root, "", encodingMap);
    std::string bitSequence;
    for (char ch : input) bitSequence += encodingMap[ch];
    std::string serializedTree;
    serializeHuffmanTree(root, serializedTree);
    uint32_t treeSize = serializedTree.size(), bitStreamLength = bitSequence.size();
    std::string compressedData = std::bitset<32>(bitStreamLength).to_string() + bitSequence;
    destroyHuffmanTree(root);
    return std::bitset<32>(treeSize).to_string() + serializedTree + compressedData;
}

std::string decompress(const std::string& compressedData) {
    if (compressedData.size() < 64) return "";
    size_t position = 0;
    uint32_t treeSize = std::bitset<32>(compressedData.substr(position, 32)).to_ulong(); position += 32;
    uint32_t bitCount = std::bitset<32>(compressedData.substr(position, 32)).to_ulong(); position += 32;
    HuffmanNode* root = deserializeHuffmanTree(compressedData.substr(position, treeSize), position);
    std::string bitStream = compressedData.substr(position, bitCount);
    std::string decodedMessage;
    HuffmanNode* currentNode = root;
    for (char bit : bitStream) {
        currentNode = (bit == '0') ? currentNode->leftChild : currentNode->rightChild;
        if (!currentNode->leftChild && !currentNode->rightChild) {
            decodedMessage += currentNode->symbol;
            currentNode = root;
        }
    }
    destroyHuffmanTree(root);
    return decodedMessage;
}
