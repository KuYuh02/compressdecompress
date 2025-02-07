#include <iostream>
#include <unordered_map>
#include <map>
#include <queue>
#include <vector>
#include <string>

// Structure for Huffman Tree node
struct HuffmanNode {
    char symbol;
    int count;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;
    
    HuffmanNode(char sym, int freq) : symbol(sym), count(freq), leftChild(nullptr), rightChild(nullptr) {}
};

// Comparator for the priority queue
struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->count > b->count;
    }
};

// Function to compute character frequencies
std::map<char, int> computeFrequencies(const std::string& input) {
    std::map<char, int> frequencyMap;
    for (char ch : input) {
        frequencyMap[ch]++;
    }
    return frequencyMap;
}

// Recursive function to generate Huffman codes
void generateHuffmanCodes(HuffmanNode* node, const std::string& code, std::unordered_map<char, std::string>& codeMap) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        codeMap[node->symbol] = code;
    }
    generateHuffmanCodes(node->leftChild, code + "0", codeMap);
    generateHuffmanCodes(node->rightChild, code + "1", codeMap);
}

// Function to construct Huffman Tree
HuffmanNode* buildHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> pq;
    
    for (const auto& entry : freqMap) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->count + right->count);
        parent->leftChild = left;
        parent->rightChild = right;
        pq.push(parent);
    }
    return pq.top();
}

// Serialize Huffman Tree
void serializeHuffmanTree(HuffmanNode* node, std::vector<unsigned char>& output) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        output.push_back(1);
        output.push_back(static_cast<unsigned char>(node->symbol));
    } else {
        output.push_back(0);
        serializeHuffmanTree(node->leftChild, output);
        serializeHuffmanTree(node->rightChild, output);
    }
}

// Deserialize Huffman Tree
HuffmanNode* deserializeHuffmanTree(const std::vector<unsigned char>& data, size_t& position) {
    if (position >= data.size()) return nullptr;
    
    if (data[position] == 1) {
        position++;
        char ch = static_cast<char>(data[position++]);
        return new HuffmanNode(ch, 0);
    }
    
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeHuffmanTree(data, position);
    node->rightChild = deserializeHuffmanTree(data, position);
    return node;
}

// Function to compress text using Huffman coding
std::string compress(const std::string& source) {
    std::map<char, int> frequencies = computeFrequencies(source);
    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    
    std::vector<unsigned char> treeStructure;
    serializeHuffmanTree(root, treeStructure);
    
    std::string encodedBits;
    for (char c : source) {
        encodedBits += huffmanCodes[c];
    }
    
    std::string compressedData(treeStructure.begin(), treeStructure.end());
    compressedData += encodedBits;
    return compressedData;
}

// Function to decompress Huffman-encoded text
std::string decompress(const std::string& source) {
    size_t position = 0;
    std::vector<unsigned char> data(source.begin(), source.end());
    HuffmanNode* root = deserializeHuffmanTree(data, position);
    
    std::string outputText;
    HuffmanNode* current = root;
    while (position < data.size()) {
        unsigned char byte = data[position];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->rightChild : current->leftChild;
            if (!current->leftChild && !current->rightChild) {
                outputText += current->symbol;
                current = root;
            }
        }
        position++;
    }
    return outputText;
}
