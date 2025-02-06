#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <vector>
#include <sstream>

// Most common words dictionary with assigned special binary codes
std::unordered_map<std::string, std::string> commonWords = {
    {"the", "000"}, {"and", "001"}, {"to", "010"}, {"of", "011"},
    {"a", "100"}, {"in", "101"}, {"is", "110"}, {"it", "111"}
};

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

// Helper function to clean and preprocess input
std::string cleanInput(const std::string& input) {
    std::string cleaned;
    for (char c : input) {
        if (isalnum(c) || c == ' ') {
            cleaned += c;
        }
    }
    return cleaned;
}

// Function to build the frequency map
std::map<char, int> buildFrequencyMap(const std::string& input) {
    std::map<char, int> frequencyMap;
    for (char c : input) {
        frequencyMap[c]++;
    }
    return frequencyMap;
}

// Generate Huffman codes recursively
void generateHuffmanCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& huffmanCodes) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
    }
    
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Build the Huffman Tree
Node* buildHuffmanTree(const std::map<char, int>& frequencyMap) {
    auto cmp = [](Node* left, Node* right) { return left->freq > right->freq; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> minHeap(cmp);

    for (const auto& pair : frequencyMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }

    return minHeap.top();
}

// Function to compress input using multi-layer compression
std::string compress(const std::string& input) {
    std::string cleanedInput = cleanInput(input);
    std::istringstream stream(cleanedInput);
    std::string word, encodedText;
    
    // Encode common words first
    while (stream >> word) {
        if (commonWords.count(word)) {
            encodedText += "W" + commonWords[word];  // Prefix 'W' for word encoding
        } else {
            for (char c : word) {
                encodedText += std::bitset<8>(c).to_string();
            }
        }
        encodedText += " ";
    }
    
    // Build Huffman encoding for remaining characters
    std::map<char, int> frequencyMap = buildFrequencyMap(encodedText);
    Node* root = buildHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    
    std::string compressed;
    for (char c : encodedText) {
        compressed += huffmanCodes[c];
    }
    
    return compressed;
}

// Function to decompress the string
std::string decompress(const std::string& compressed) {
    std::string decodedBinary, decodedText, buffer;
    std::istringstream stream(compressed);
    std::string bitSequence;
    
    // Decode Huffman-encoded text back to binary
    while (stream >> bitSequence) {
        decodedBinary += bitSequence;
    }
    
    std::istringstream binaryStream(decodedBinary);
    std::string token;
    while (binaryStream >> token) {
        if (token[0] == 'W') {
            std::string wordCode = token.substr(1);
            for (const auto& pair : commonWords) {
                if (pair.second == wordCode) {
                    decodedText += pair.first + " ";
                    break;
                }
            }
        } else {
            char decodedChar = static_cast<char>(std::bitset<8>(token).to_ulong());
            decodedText += decodedChar;
        }
    }
    
    return decodedText;
}
