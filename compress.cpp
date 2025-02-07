#include <iostream>
#include <unordered_map>
#include <map>
#include <queue>
#include <vector>
#include <string>

// Structure for a Huffman Tree node
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
};

// Comparator for the priority queue
struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to calculate character frequencies
std::map<char, int> calculateFrequencies(const std::string& input) {
    std::map<char, int> frequencyTable;
    for (char ch : input) {
        frequencyTable[ch]++;
    }
    return frequencyTable;
}

// Function to assign Huffman codes recursively
void assignHuffmanCodes(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& codeTable) {
    if (!node) return;
    if (!node->left && !node->right) {
        codeTable[node->character] = path;
    }
    assignHuffmanCodes(node->left, path + "0", codeTable);
    assignHuffmanCodes(node->right, path + "1", codeTable);
}

// Function to create Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> priorityQueue;
    
    for (const auto& pair : frequencies) {
        priorityQueue.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (priorityQueue.size() > 1) {
        HuffmanNode* leftNode = priorityQueue.top(); priorityQueue.pop();
        HuffmanNode* rightNode = priorityQueue.top(); priorityQueue.pop();
        HuffmanNode* parentNode = new HuffmanNode('\0', leftNode->frequency + rightNode->frequency);
        parentNode->left = leftNode;
        parentNode->right = rightNode;
        priorityQueue.push(parentNode);
    }
    return priorityQueue.top();
}

// Serialize Huffman Tree
void encodeHuffmanTree(HuffmanNode* node, std::vector<unsigned char>& treeData) {
    if (!node) return;
    if (!node->left && !node->right) {
        treeData.push_back(1);
        treeData.push_back(static_cast<unsigned char>(node->character));
    } else {
        treeData.push_back(0);
        encodeHuffmanTree(node->left, treeData);
        encodeHuffmanTree(node->right, treeData);
    }
}

// Deserialize Huffman Tree
HuffmanNode* decodeHuffmanTree(const std::vector<unsigned char>& treeData, size_t& position) {
    if (position >= treeData.size()) return nullptr;
    
    if (treeData[position] == 1) {
        position++;
        char ch = static_cast<char>(treeData[position++]);
        return new HuffmanNode(ch, 0);
    }
    
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeHuffmanTree(treeData, position);
    node->right = decodeHuffmanTree(treeData, position);
    return node;
}

// Function to compress data using Huffman coding
std::string compress(const std::string& source) {
    std::map<char, int> frequencies = calculateFrequencies(source);
    HuffmanNode* root = constructHuffmanTree(frequencies);
    std::unordered_map<char, std::string> huffmanTable;
    assignHuffmanCodes(root, "", huffmanTable);
    
    std::vector<unsigned char> treeStructure;
    encodeHuffmanTree(root, treeStructure);
    
    std::string encodedBits;
    for (char c : source) {
        encodedBits += huffmanTable[c];
    }
    
    std::string compressedData(treeStructure.begin(), treeStructure.end());
    compressedData += encodedBits;
    return compressedData;
}

// Function to decompress Huffman-encoded data
std::string decompress(const std::string& source) {
    size_t position = 0;
    std::vector<unsigned char> treeData(source.begin(), source.end());
    HuffmanNode* root = decodeHuffmanTree(treeData, position);
    
    std::string outputText;
    HuffmanNode* currentNode = root;
    while (position < treeData.size()) {
        unsigned char byte = treeData[position];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            currentNode = bit ? currentNode->right : currentNode->left;
            if (!currentNode->left && !currentNode->right) {
                outputText += currentNode->character;
                currentNode = root;
            }
        }
        position++;
    }
    return outputText;
}
