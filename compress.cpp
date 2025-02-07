#include <iostream>
#include <unordered_map>
#include <map>
#include <queue>
#include <vector>
#include <string>

// Node structure for Huffman Tree
struct HuffmanNode {
    char character;
    int freq;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;
    
    HuffmanNode(char ch, int frequency) : character(ch), freq(frequency), leftChild(nullptr), rightChild(nullptr) {}
};

// Comparator for priority queue
struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// Function to calculate character frequencies
std::map<char, int> computeCharFrequencies(const std::string& text) {
    std::map<char, int> frequencyMap;
    for (char c : text) {
        frequencyMap[c]++;
    }
    return frequencyMap;
}

// Recursive function to assign Huffman codes
void assignHuffmanCodes(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& huffmanMap) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        huffmanMap[node->character] = path;
    }
    assignHuffmanCodes(node->leftChild, path + "0", huffmanMap);
    assignHuffmanCodes(node->rightChild, path + "1", huffmanMap);
}

// Function to build the Huffman Tree
HuffmanNode* createHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> pq;
    
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->leftChild = left;
        parent->rightChild = right;
        pq.push(parent);
    }
    return pq.top();
}

// Serialize Huffman Tree
void serializeTree(HuffmanNode* node, std::vector<unsigned char>& output) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        output.push_back(1);
        output.push_back(static_cast<unsigned char>(node->character));
    } else {
        output.push_back(0);
        serializeTree(node->leftChild, output);
        serializeTree(node->rightChild, output);
    }
}

// Deserialize Huffman Tree
HuffmanNode* deserializeTree(const std::vector<unsigned char>& data, size_t& position) {
    if (position >= data.size()) return nullptr;
    
    if (data[position] == 1) {
        position++;
        char ch = static_cast<char>(data[position++]);
        return new HuffmanNode(ch, 0);
    }
    
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeTree(data, position);
    node->rightChild = deserializeTree(data, position);
    return node;
}

// Encoding function
std::string compressText(const std::string& text) {
    std::map<char, int> frequencies = computeCharFrequencies(text);
    HuffmanNode* root = createHuffmanTree(frequencies);
    std::unordered_map<char, std::string> huffmanCodes;
    assignHuffmanCodes(root, "", huffmanCodes);
    
    std::vector<unsigned char> serializedTree;
    serializeTree(root, serializedTree);
    
    std::string bitRepresentation;
    for (char c : text) {
        bitRepresentation += huffmanCodes[c];
    }
    
    std::string compressedData(serializedTree.begin(), serializedTree.end());
    compressedData += bitRepresentation;
    return compressedData;
}

// Decoding function
std::string decompressText(const std::string& encodedData) {
    size_t pos = 0;
    std::vector<unsigned char> data(encodedData.begin(), encodedData.end());
    HuffmanNode* root = deserializeTree(data, pos);
    
    std::string decodedOutput;
    HuffmanNode* current = root;
    while (pos < data.size()) {
        unsigned char byte = data[pos];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->rightChild : current->leftChild;
            if (!current->leftChild && !current->rightChild) {
                decodedOutput += current->character;
                current = root;
            }
        }
        pos++;
    }
    return decodedOutput;
}
