#include <iostream>
#include <map>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <string>
#include <sstream>
#include <vector>

// Node structure for Huffman Tree
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
    bool operator>(const Node& other) const {
        return freq > other.freq;
    }
};

// Helper function to build the frequency map for a string
std::map<char, int> buildFrequencyMap(const std::string& input) {
    std::map<char, int> frequencyMap;
    for (char c : input) {
        frequencyMap[c]++;
    }
    return frequencyMap;
}

// Helper function to generate Huffman codes
void generateHuffmanCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& huffmanCodes) {
    if (root == nullptr) return;
    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
    }
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Function to build the Huffman Tree from a frequency map
Node* buildHuffmanTree(const std::map<char, int>& frequencyMap) {
    auto cmp = [](Node* left, Node* right) { return left->freq > right->freq; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> minHeap(cmp);

    for (const auto& pair : frequencyMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();
        Node* internalNode = new Node('\0', left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;
        minHeap.push(internalNode);
    }

    return minHeap.top();  // Root of the Huffman Tree
}

// Helper function to serialize the tree into a compact binary format (byte packing)
void serializeTree(Node* root, std::vector<unsigned char>& treeData) {
    if (!root) return;
    if (!root->left && !root->right) { // Leaf node
        treeData.push_back(1);  // Mark as a leaf node
        treeData.push_back(static_cast<unsigned char>(root->data));  // Store the character
    } else {  // Internal node
        treeData.push_back(0);  // Mark as an internal node
        serializeTree(root->left, treeData);
        serializeTree(root->right, treeData);
    }
}

// Helper function to deserialize the tree from the binary format (byte packing)
Node* deserializeTree(const std::vector<unsigned char>& treeData, size_t& index) {
    if (index >= treeData.size()) return nullptr;

    if (treeData[index] == 1) {  // Leaf node
        index++;
        char data = static_cast<char>(treeData[index]);
        index++;
        return new Node(data, 0);
    }

    index++;
    Node* internalNode = new Node('\0', 0);
    internalNode->left = deserializeTree(treeData, index);
    internalNode->right = deserializeTree(treeData, index);
    return internalNode;
}

// Function to replace special characters with single-byte values
std::string replaceSpecialCharacters(const std::string& input) {
    std::string output = input;
    
    // Replace special characters with single-byte values
    const std::unordered_map<char, unsigned char> replacements = {
        {'\n', static_cast<unsigned char>(0x80)}, {'\"', static_cast<unsigned char>(0x81)}, 
        {'$', static_cast<unsigned char>(0x82)}, {'%', static_cast<unsigned char>(0x83)}, 
        {'&', static_cast<unsigned char>(0x84)}, {'(', static_cast<unsigned char>(0x85)}, 
        {')', static_cast<unsigned char>(0x86)}, {'*', static_cast<unsigned char>(0x87)}, 
        {',', static_cast<unsigned char>(0x88)}, {'-', static_cast<unsigned char>(0x89)}, 
        {'.', static_cast<unsigned char>(0x8A)}, {'\'', static_cast<unsigned char>(0x8B)}
    };
    
    for (auto& ch : output) {
        if (replacements.find(ch) != replacements.end()) {
            ch = static_cast<char>(replacements.at(ch));
        }
    }
    
    return output;
}

// Function to revert special character byte values back to original characters
std::string revertSpecialCharacters(const std::string& input) {
    std::string output = input;
    
    // Reverse the replacements with original characters
    const std::unordered_map<unsigned char, char> replacements = {
        {0x80, '\n'}, {0x81, '\"'}, {0x82, '$'}, {0x83, '%'}, 
        {0x84, '&'}, {0x85, '('}, {0x86, ')'}, {0x87, '*'}, 
        {0x88, ','}, {0x89, '-'}, {0x8A, '.'}, {0x8B, '\''}
    };
    
    for (auto& ch : output) {
        unsigned char byteVal = static_cast<unsigned char>(ch);
        if (replacements.find(byteVal) != replacements.end()) {
            ch = replacements.at(byteVal);
        }
    }
    
    return output;
}

// Function to compress a single string using Huffman coding with bit-packing
std::string compress(const std::string& source) {
    // Step 1: Replace special characters before compression
    std::string modifiedSource = replaceSpecialCharacters(source);
    
    // Step 2: Build frequency map and Huffman tree for the modified string
    std::map<char, int> frequencyMap = buildFrequencyMap(modifiedSource);
    Node* root = buildHuffmanTree(frequencyMap);

    // Generate Huffman codes for the string
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Serialize the Huffman tree
    std::vector<unsigned char> serializedTree;
    serializeTree(root, serializedTree);

    // Step 3: Compress the modified input string using Huffman codes (bit-packing)
    std::string compressedData;
    unsigned char currentByte = 0;
    int bitCount = 0;

    for (char c : modifiedSource) {
        std::string code = huffmanCodes[c];
        for (char bit : code) {
            currentByte <<= 1;
            if (bit == '1') {
                currentByte |= 1;
            }
            bitCount++;

            if (bitCount == 8) {
                compressedData.push_back(currentByte);
                currentByte = 0;
                bitCount = 0;
            }
        }
    }

    if (bitCount > 0) {
        currentByte <<= (8 - bitCount);  // Pad remaining bits with 0
        compressedData.push_back(currentByte);
    }

    // Step 4: Combine the tree and the compressed data
    std::string finalCompressedData(serializedTree.begin(), serializedTree.end());
    finalCompressedData += compressedData;

    return finalCompressedData;
}

// Function to decompress the string using the Huffman Tree
std::string decompress(const std::string& source) {
    size_t index = 0;

    // Convert the string to a vector of unsigned chars for easier deserialization
    std::vector<unsigned char> treeData(source.begin(), source.end());

    // Deserialize the Huffman tree from the compressed data
    Node* root = nullptr;
    root = deserializeTree(treeData, index);

    std::string decompressed;
    Node* current = root;

    // Step 1: Decompress the binary data using the Huffman tree (bit-unpacking)
    while (index < treeData.size()) {
        unsigned char byte = treeData[index];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;

            // If we've reached a leaf node, add the character to the decompressed string
            if (!current->left && !current->right) {
                decompressed += current->data;
                current = root;  // Reset to root for the next character
            }
        }
        index++;
    }

    // Step 2: Reverse the special character replacements
    return revertSpecialCharacters(decompressed);
}
