#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Structure representing a node in the Huffman Tree
struct HuffmanNode {
    char symbol;
    int frequency;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

    HuffmanNode(char sym, int freq) : symbol(sym), frequency(freq), leftChild(nullptr), rightChild(nullptr) {}
    bool operator>(const HuffmanNode& other) const {
        return frequency > other.frequency;
    }
};

// Function to create a frequency map from input text
std::map<char, int> computeFrequency(const std::string& text) {
    std::map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }
    return freqMap;
}

// Function to recursively assign Huffman codes to characters
void assignCodes(HuffmanNode* node, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!node) return;

    if (!node->leftChild && !node->rightChild) { // Leaf node
        codes[node->symbol] = code;
    }

    assignCodes(node->leftChild, code + "0", codes);
    assignCodes(node->rightChild, code + "1", codes);
}

// Function to construct the Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& freqMap) {
    auto comparator = [](HuffmanNode* left, HuffmanNode* right) { return left->frequency > right->frequency; };
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(comparator)> minHeap(comparator);

    for (const auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->leftChild = left;
        parent->rightChild = right;
        minHeap.push(parent);
    }

    return minHeap.top();
}

// Function to serialize the Huffman tree
std::string encodeTree(HuffmanNode* node) {
    if (!node) return "";

    if (!node->leftChild && !node->rightChild) {
        return "1" + std::bitset<8>(node->symbol).to_string();
    }
    return "0" + encodeTree(node->leftChild) + encodeTree(node->rightChild);
}

// Function to deserialize the Huffman tree
HuffmanNode* decodeTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;

    if (data[pos] == '1') {
        pos++;
        char symbol = static_cast<char>(std::bitset<8>(data.substr(pos, 8)).to_ulong());
        pos += 8;
        return new HuffmanNode(symbol, 0);
    }

    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = decodeTree(data, pos);
    node->rightChild = decodeTree(data, pos);
    return node;
}

// Function to compress text using Huffman coding
std::string compressText(const std::string& text) {
    std::map<char, int> frequencies = computeFrequency(text);
    HuffmanNode* root = constructHuffmanTree(frequencies);
    
    std::unordered_map<char, std::string> codes;
    assignCodes(root, "", codes);

    std::string serializedTree = encodeTree(root);
    std::string compressedText;
    for (char ch : text) {
        compressedText += codes[ch];
    }

    return serializedTree + compressedText;
}

// Function to decompress Huffman encoded text
std::string decompressText(const std::string& encodedData) {
    size_t pos = 0;
    HuffmanNode* root = decodeTree(encodedData, pos);
    
    std::string originalText;
    HuffmanNode* currentNode = root;
    while (pos < encodedData.size()) {
        currentNode = (encodedData[pos] == '0') ? currentNode->leftChild : currentNode->rightChild;
        pos++;
        
        if (!currentNode->leftChild && !currentNode->rightChild) {
            originalText += currentNode->symbol;
            currentNode = root;
        }
    }
    
    return originalText;
}
