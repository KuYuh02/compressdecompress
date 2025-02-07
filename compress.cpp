#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Structure representing a node in the Huffman Tree
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to build a frequency map from the input string
std::map<char, int> computeFrequencyMap(const std::string& text) {
    std::map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }
    return freqMap;
}

// Recursive function to generate Huffman codes
void createHuffmanCodes(HuffmanNode* node, const std::string& prefix, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        codes[node->character] = prefix;
    }
    
    createHuffmanCodes(node->left, prefix + "0", codes);
    createHuffmanCodes(node->right, prefix + "1", codes);
}

// Function to construct the Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;
    
    for (const auto& pair : freqMap) {
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

// Function to serialize the Huffman tree
std::string encodeTree(HuffmanNode* node) {
    if (!node) return "";
    if (!node->left && !node->right) {
        return "1" + std::bitset<8>(node->character).to_string();
    }
    return "0" + encodeTree(node->left) + encodeTree(node->right);
}

// Function to deserialize the Huffman tree
HuffmanNode* decodeTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;
    
    if (data[pos] == '1') {
        pos++;
        char ch = static_cast<char>(std::bitset<8>(data.substr(pos, 8)).to_ulong());
        pos += 8;
        return new HuffmanNode(ch, 0);
    }
    
    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(data, pos);
    node->right = decodeTree(data, pos);
    return node;
}

// Function to compress a given string
std::string compressText(const std::string& text) {
    std::map<char, int> frequencies = computeFrequencyMap(text);
    HuffmanNode* root = constructHuffmanTree(frequencies);
    
    std::unordered_map<char, std::string> codes;
    createHuffmanCodes(root, "", codes);
    
    std::string serializedTree = encodeTree(root);
    std::string encodedText;
    for (char ch : text) {
        encodedText += codes[ch];
    }
    
    return serializedTree + encodedText;
}

// Function to decompress Huffman encoded text
std::string decompressText(const std::string& encodedData) {
    size_t pos = 0;
    HuffmanNode* root = decodeTree(encodedData, pos);
    
    std::string originalText;
    HuffmanNode* currentNode = root;
    while (pos < encodedData.size()) {
        currentNode = (encodedData[pos] == '0') ? currentNode->left : currentNode->right;
        pos++;
        
        if (!currentNode->left && !currentNode->right) {
            originalText += currentNode->character;
            currentNode = root;
        }
    }
    
    return originalText;
}
