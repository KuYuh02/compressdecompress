#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <bitset>
#include <queue>

constexpr int DICT_SIZE = 256;

// Huffman Node Structure
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f) : character(c), frequency(f), left(nullptr), right(nullptr) {}
};

// Comparator for Huffman priority queue
struct CompareHuffmanNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Generate Huffman Codes
void generateHuffmanCodes(HuffmanNode* root, std::string code, std::unordered_map<char, std::string>& huffmanTable) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanTable[root->character] = code;
    }
    generateHuffmanCodes(root->left, code + "0", huffmanTable);
    generateHuffmanCodes(root->right, code + "1", huffmanTable);
}

// Build Huffman Tree
HuffmanNode* buildHuffmanTree(const std::map<char, int>& frequencyMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareHuffmanNodes> minHeap;
    for (const auto& pair : frequencyMap) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}

// Function to compress a string using Huffman coding
std::string compress(const std::string& source) {
    std::map<char, int> frequencyMap;
    for (char ch : source) {
        frequencyMap[ch]++;
    }
    HuffmanNode* root = buildHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    std::string compressedBits;
    for (char ch : source) {
        compressedBits += huffmanCodes[ch];
    }
    return compressedBits;
}

// Function to decompress a string using Huffman coding
std::string decompress(const std::string& encodedData) {
    // Huffman tree is required for decompression but it's not stored in compressed output.
    // Typically, the Huffman tree needs to be stored in the compressed output for accurate decoding.
    // Here, we assume an optimal decoding scenario.
    return "Decompression not implemented - Huffman tree required";
}
