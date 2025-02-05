#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Huffman Tree Node
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

// Generate frequency map
std::map<char, int> computeFrequencies(const std::string& text) {
    std::map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }
    return freqMap;
}

// Build Huffman tree
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

// Generate Huffman codes recursively
void assignHuffmanCodes(HuffmanNode* node, std::string currentCode, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        codes[node->character] = currentCode;
    }

    assignHuffmanCodes(node->left, currentCode + "0", codes);
    assignHuffmanCodes(node->right, currentCode + "1", codes);
}

// Serialize Huffman tree
std::string encodeTree(HuffmanNode* node) {
    if (!node) return "";

    if (!node->left && !node->right) {
        return "1" + std::bitset<8>(node->character).to_string();
    }
    return "0" + encodeTree(node->left) + encodeTree(node->right);
}

// Deserialize Huffman tree
HuffmanNode* decodeTree(const std::string& treeData, size_t& pos) {
    if (pos >= treeData.size()) return nullptr;

    if (treeData[pos] == '1') {
        pos++;
        char ch = static_cast<char>(std::bitset<8>(treeData.substr(pos, 8)).to_ulong());
        pos += 8;
        return new HuffmanNode(ch, 0);
    }
    
    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(treeData, pos);
    node->right = decodeTree(treeData, pos);
    return node;
}

// Compress a string using Huffman coding
std::string compressString(const std::string& text) {
    std::map<char, int> frequencies = computeFrequencies(text);
    HuffmanNode* root = constructHuffmanTree(frequencies);
    
    std::unordered_map<char, std::string> codes;
    assignHuffmanCodes(root, "", codes);

    std::string encodedTree = encodeTree(root);
    std::string encodedText;
    for (char ch : text) {
        encodedText += codes[ch];
    }

    return encodedTree + encodedText;
}

// Decompress a Huffman encoded string
std::string decompressString(const std::string& encoded) {
    size_t index = 0;
    HuffmanNode* root = decodeTree(encoded, index);
    
    std::string originalText;
    HuffmanNode* currentNode = root;
    while (index < encoded.size()) {
        currentNode = (encoded[index] == '0') ? currentNode->left : currentNode->right;
        index++;
        
        if (!currentNode->left && !currentNode->right) {
            originalText += currentNode->character;
            currentNode = root;
        }
    }
    
    return originalText;
}
