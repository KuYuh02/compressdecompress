#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <sstream>

// Struct to define a node for Huffman Tree
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r) : ch('\0'), freq(f), left(l), right(r) {}
};

// Comparison operator for priority queue
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }
};

// Predefined special words and their codes
std::unordered_map<std::string, std::string> wordCodes = {
    {"the", "0001"}, {"and", "0010"}, {"a", "0011"}, {"to", "0100"},
    {"is", "0101"}, {"in", "0110"}, {"it", "0111"}, {"you", "1000"},
    {"that", "1001"}, {"of", "1010"}, {"for", "1011"},
    // Add more common words as needed
};

// Helper function to generate Huffman codes
void generateCodes(HuffmanNode* root, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    if (root->ch != '\0') {
        codes[root->ch] = code; // leaf node, save the code
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Function to build the Huffman tree for characters
std::string buildHuffmanTreeAndEncode(const std::string& source, std::unordered_map<char, std::string>& huffmanCodes) {
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
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();
        HuffmanNode* newNode = new HuffmanNode(left->freq + right->freq, left, right);
        minHeap.push(newNode);
    }

    // Step 3: Generate Huffman codes from the tree
    generateCodes(minHeap.top(), "", huffmanCodes);

    // Step 4: Encode the input string using Huffman codes
    std::stringstream compressed;
    for (char ch : source) {
        compressed << huffmanCodes[ch];
    }

    return compressed.str();
}

// Function to replace common words with special codes
std::string replaceCommonWordsWithCodes(const std::string& input) {
    std::stringstream modified;
    std::string word;
    std::istringstream stream(input);

    while (stream >> word) {
        // Replace words with special codes if present in the wordCodes map
        if (wordCodes.find(word) != wordCodes.end()) {
            modified << wordCodes[word] << " ";  // Add special code for the word
        } else {
            modified << word << " ";  // Otherwise, keep the word as it is
        }
    }

    return modified.str();
}

// Decompress function
std::string decompress(const std::string& compressed, const std::unordered_map<std::string, char>& reverseHuffmanCodes) {
    std::stringstream decompressed;
    std::string code = "";

    for (char bit : compressed) {
        code += bit;
        if (reverseHuffmanCodes.find(code) != reverseHuffmanCodes.end()) {
            decompressed << reverseHuffmanCodes.at(code);
            code = ""; // Reset for next character
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

// Function to combine all compression layers
std::string compress(const std::string& source, std::unordered_map<char, std::string>& huffmanCodes) {
    // First replace common words with special codes
    std::string modified = replaceCommonWordsWithCodes(source);
    
    // Then build the Huffman tree and encode the remaining string
    return buildHuffmanTreeAndEncode(modified, huffmanCodes);
}
