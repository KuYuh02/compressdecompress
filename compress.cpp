#include <iostream>
#include <unordered_map>
#include <map>
#include <queue>
#include <vector>
#include <string>

// Structure representing a node in the Huffman tree
struct HuffmanNode {
    char symbol;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char sym, int freq) : symbol(sym), frequency(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to compute frequency of characters in input
std::map<char, int> computeFrequencies(const std::string& text) {
    std::map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }
    return freqMap;
}

// Recursive function to generate Huffman codes
void createHuffmanCodes(HuffmanNode* node, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        codes[node->symbol] = code;
    }
    createHuffmanCodes(node->left, code + "0", codes);
    createHuffmanCodes(node->right, code + "1", codes);
}

// Function to construct the Huffman tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    
    for (const auto& entry : frequencies) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

// Serialization of Huffman tree
void serializeHuffmanTree(HuffmanNode* node, std::vector<unsigned char>& output) {
    if (!node) return;
    if (!node->left && !node->right) {
        output.push_back(1);
        output.push_back(static_cast<unsigned char>(node->symbol));
    } else {
        output.push_back(0);
        serializeHuffmanTree(node->left, output);
        serializeHuffmanTree(node->right, output);
    }
}

// Deserialization of Huffman tree
HuffmanNode* deserializeHuffmanTree(const std::vector<unsigned char>& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    
    if (data[index] == 1) {
        index++;
        char symbol = static_cast<char>(data[index++]);
        return new HuffmanNode(symbol, 0);
    }
    
    index++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = deserializeHuffmanTree(data, index);
    node->right = deserializeHuffmanTree(data, index);
    return node;
}

// Encoding function
std::string encodeText(const std::string& text) {
    std::map<char, int> frequencies = computeFrequencies(text);
    HuffmanNode* root = constructHuffmanTree(frequencies);
    std::unordered_map<char, std::string> huffmanCodes;
    createHuffmanCodes(root, "", huffmanCodes);
    
    std::vector<unsigned char> treeData;
    serializeHuffmanTree(root, treeData);
    
    std::string bitString;
    for (char ch : text) {
        bitString += huffmanCodes[ch];
    }
    
    std::string compressedData(treeData.begin(), treeData.end());
    compressedData += bitString;
    return compressedData;
}

// Decoding function
std::string decodeText(const std::string& encodedData) {
    size_t index = 0;
    std::vector<unsigned char> data(encodedData.begin(), encodedData.end());
    HuffmanNode* root = deserializeHuffmanTree(data, index);
    
    std::string decodedText;
    HuffmanNode* current = root;
    while (index < data.size()) {
        unsigned char byte = data[index];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                decodedText += current->symbol;
                current = root;
            }
        }
        index++;
    }
    return decodedText;
}
