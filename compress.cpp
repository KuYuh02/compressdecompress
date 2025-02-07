#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <bitset>
#include <queue>
#include <sstream>

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

// Serialize Huffman Tree
std::string serializeHuffmanTree(HuffmanNode* root) {
    if (!root) return "";
    if (!root->left && !root->right) {
        return "1" + std::bitset<8>(root->character).to_string();
    }
    return "0" + serializeHuffmanTree(root->left) + serializeHuffmanTree(root->right);
}

// Deserialize Huffman Tree
HuffmanNode* deserializeHuffmanTree(const std::string& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    if (data[index] == '1') {
        index++;
        char ch = static_cast<char>(std::bitset<8>(data.substr(index, 8)).to_ulong());
        index += 8;
        return new HuffmanNode(ch, 0);
    }
    index++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = deserializeHuffmanTree(data, index);
    node->right = deserializeHuffmanTree(data, index);
    return node;
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
    std::string serializedTree = serializeHuffmanTree(root);
    std::string compressedBits;
    for (char ch : source) {
        compressedBits += huffmanCodes[ch];
    }
    return serializedTree + "|" + compressedBits;
}

// Function to decompress a string using Huffman coding
std::string decompress(const std::string& encodedData) {
    size_t separator = encodedData.find('|');
    if (separator == std::string::npos) return "ERROR: Invalid encoding";
    std::string treeData = encodedData.substr(0, separator);
    std::string bitstream = encodedData.substr(separator + 1);
    
    size_t index = 0;
    HuffmanNode* root = deserializeHuffmanTree(treeData, index);
    std::string decodedText;
    HuffmanNode* current = root;
    for (char bit : bitstream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decodedText += current->character;
            current = root;
        }
    }
    return decodedText;
}
