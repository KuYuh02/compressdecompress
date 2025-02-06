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
    std::istringstream stream(input);
    std::string word, encodedText;
    
    // Encode common words first
    while (stream >> word) {
        if (commonWords.count(word)) {
            encodedText += "W" + commonWords[word] + " ";
        } else {
            for (char c : word) {
                encodedText += c;
            }
            encodedText += " ";
        }
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
    std::string decodedText, buffer;
    
    // Decode Huffman-encoded text back to original
    std::istringstream binaryStream(compressed);
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
            decodedText += token + " ";
        }
    }
    
    return decodedText;
}
