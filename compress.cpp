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

std::unordered_map<std::string, std::string> reverseCommonWords;

// Node structure for Huffman Tree
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
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

// Serialize the Huffman tree for decoding
std::string serializeTree(Node* root) {
    if (!root) return "";
    if (!root->left && !root->right) {
        return "1" + std::bitset<8>(root->data).to_string();
    }
    return "0" + serializeTree(root->left) + serializeTree(root->right);
}

// Deserialize Huffman tree from binary string
Node* deserializeTree(const std::string& treeData, size_t& index) {
    if (index >= treeData.size()) return nullptr;
    
    if (treeData[index] == '1') {
        index++;
        char data = static_cast<char>(std::bitset<8>(treeData.substr(index, 8)).to_ulong());
        index += 8;
        return new Node(data, 0);
    }
    
    index++;
    Node* node = new Node('\0', 0);
    node->left = deserializeTree(treeData, index);
    node->right = deserializeTree(treeData, index);
    return node;
}

// Function to compress input using Huffman encoding and common words
std::string compress(const std::string& input) {
    std::istringstream stream(input);
    std::string word, encodedText;
    
    // Encode common words first
    while (stream >> word) {
        if (commonWords.count(word)) {
            encodedText += "W" + commonWords[word] + " ";
        } else {
            for (char c : word) {
                encodedText += std::bitset<8>(c).to_string();
            }
            encodedText += " ";
        }
    }
    
    // Build Huffman encoding for remaining characters
    std::map<char, int> frequencyMap = buildFrequencyMap(encodedText);
    Node* root = buildHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    
    std::string serializedTree = serializeTree(root);
    std::string compressed;
    for (char c : encodedText) {
        compressed += huffmanCodes[c];
    }
    
    return serializedTree + "|" + compressed;
}

// Function to decompress the string
std::string decompress(const std::string& compressed) {
    size_t delimiter = compressed.find('|');
    if (delimiter == std::string::npos) return "";
    
    std::string treeData = compressed.substr(0, delimiter);
    std::string encodedText = compressed.substr(delimiter + 1);
    
    size_t index = 0;
    Node* root = deserializeTree(treeData, index);
    
    std::string decodedBinary;
    Node* current = root;
    for (char c : encodedText) {
        if (c == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (!current->left && !current->right) {
            decodedBinary += current->data;
            current = root;
        }
    }
    
    std::istringstream stream(decodedBinary);
    std::string output, token;
    while (stream >> token) {
        if (token[0] == 'W' && reverseCommonWords.count(token.substr(1))) {
            output += reverseCommonWords[token.substr(1)] + " ";
        } else {
            std::string charStr;
            for (size_t i = 0; i < token.size(); i += 8) {
                char decodedChar = static_cast<char>(std::bitset<8>(token.substr(i, 8)).to_ulong());
                charStr += decodedChar;
            }
            output += charStr + " ";
        }
    }
    
    return output;
}
