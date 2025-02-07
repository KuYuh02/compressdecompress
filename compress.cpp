#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Node structure for Huffman Tree
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), leftChild(nullptr), rightChild(nullptr) {}
};

// Comparator for priority queue
struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to create frequency map
std::map<char, int> computeFrequency(const std::string& text) {
    std::map<char, int> frequency;
    for (char symbol : text) {
        frequency[symbol]++;
    }
    return frequency;
}

// Function to generate Huffman codes
void assignCodes(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& codeMap) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        codeMap[node->character] = path;
    }
    assignCodes(node->leftChild, path + "0", codeMap);
    assignCodes(node->rightChild, path + "1", codeMap);
}

// Function to construct Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& frequencyMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> heap;
    for (const auto& pair : frequencyMap) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }
    while (heap.size() > 1) {
        HuffmanNode* left = heap.top(); heap.pop();
        HuffmanNode* right = heap.top(); heap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->leftChild = left;
        parent->rightChild = right;
        heap.push(parent);
    }
    return heap.top();
}

// Function to serialize the Huffman tree
std::string serializeHuffmanTree(HuffmanNode* root) {
    if (!root) return "";
    if (!root->leftChild && !root->rightChild) {
        return "1" + std::bitset<8>(root->character).to_string();
    }
    return "0" + serializeHuffmanTree(root->leftChild) + serializeHuffmanTree(root->rightChild);
}

// Function to deserialize the Huffman tree
HuffmanNode* deserializeHuffmanTree(const std::string& data, size_t& position) {
    if (position >= data.size()) return nullptr;
    if (data[position] == '1') {
        position++;
        char symbol = static_cast<char>(std::bitset<8>(data.substr(position, 8)).to_ulong());
        position += 8;
        return new HuffmanNode(symbol, 0);
    }
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeHuffmanTree(data, position);
    node->rightChild = deserializeHuffmanTree(data, position);
    return node;
}

// Function to compress a string
std::string compressText(const std::string& input) {
    std::map<char, int> frequencyMap = computeFrequency(input);
    HuffmanNode* root = constructHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> huffmanCodes;
    assignCodes(root, "", huffmanCodes);
    std::string serializedTree = serializeHuffmanTree(root);
    std::string compressed;
    for (char symbol : input) {
        compressed += huffmanCodes[symbol];
    }
    return serializedTree + compressed;
}

// Function to decompress a string
std::string decompressText(const std::string& encodedData) {
    size_t position = 0;
    HuffmanNode* root = deserializeHuffmanTree(encodedData, position);
    std::string decodedText;
    HuffmanNode* currentNode = root;
    while (position < encodedData.size()) {
        currentNode = (encodedData[position] == '0') ? currentNode->leftChild : currentNode->rightChild;
        position++;
        if (!currentNode->leftChild && !currentNode->rightChild) {
            decodedText += currentNode->character;
            currentNode = root;
        }
    }
    return decodedText;
}
