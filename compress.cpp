#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <vector>
#include <sstream>

// Struct to define a node for Huffman Tree
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    // Constructor
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparison operator for priority queue
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }
};

// Generate Huffman codes from the tree
void generateCodes(HuffmanNode* root, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    if (root->ch != '\0') {
        codes[root->ch] = code; // leaf node, save the code
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Compress the input string
std::string compress(const std::string& source) {
    if (source.empty()) return "";

    // Step 1: Calculate frequencies of each character
    std::unordered_map<char, int> freqMap;
    for (char ch : source) {
        freqMap[ch]++;
    }

    // Step 2: Create a priority queue (min-heap) and build the Huffman tree
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> minHeap;
    for (auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }

    while (minHeap.size() > 1) {
        // Take two nodes with the smallest frequency
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();

        // Create a new internal node with these two as children
        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        minHeap.push(newNode);
    }

    // Step 3: Generate Huffman codes from the tree
    std::unordered_map<char, std::string> huffmanCodes;
    generateCodes(minHeap.top(), "", huffmanCodes);

    // Step 4: Encode the input string using Huffman codes
    std::stringstream compressed;
    for (char ch : source) {
        compressed << huffmanCodes[ch];
    }

    // Return the compressed bitstream
    return compressed.str();
}

// Decompress the input string using the Huffman tree
std::string decompress(const std::string& compressed, const std::unordered_map<std::string, char>& reverseHuffmanCodes) {
    std::stringstream decompressed;
    std::string code = "";
    
    for (char bit : compressed) {
        code += bit;
        if (reverseHuffmanCodes.find(code) != reverseHuffmanCodes.end()) {
            decompressed << reverseHuffmanCodes.at(code);
            code = "";
        }
    }

    return decompressed.str();
}

// Function to generate the reverse Huffman code map
std::unordered_map<std::string, char> generateReverseCodes(const std::unordered_map<char, std::string>& huffmanCodes) {
    std::unordered_map<std::string, char> reverseHuffmanCodes;
    for (auto& entry : huffmanCodes) {
        reverseHuffmanCodes[entry.second] = entry.first;
    }
    return reverseHuffmanCodes;
}

