#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <vector>

// Common words dictionary
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

// Helper function to remove irrelevant characters
std::string cleanInput(const std::string& input) {
    std::string cleaned;
    for (char c : input) {
        if (isalnum(c) || c == ' ') {
            cleaned += c;
        }
    }
    return cleaned;
}

// Helper function to build the frequency map
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

    if (!root->left && !root->right) {  // Leaf node
        huffmanCodes[root->data] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Function to build the Huffman Tree
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

    return minHeap.top();
}

// Function to compress the string using Huffman codes and common words encoding
std::string compress(const std::string& input) {
    std::string cleanedInput = cleanInput(input);
    std::map<char, int> frequencyMap = buildFrequencyMap(cleanedInput);
    Node* root = buildHuffmanTree(frequencyMap);

    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    std::string compressed;
    std::string word;
    for (char c : cleanedInput) {
        if (c == ' ') {
            if (!word.empty() && commonWords.count(word)) {
                compressed += commonWords[word];
            } else {
                for (char wc : word) {
                    compressed += huffmanCodes[wc];
                }
            }
            compressed += " ";
            word.clear();
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        if (commonWords.count(word)) {
            compressed += commonWords[word];
        } else {
            for (char wc : word) {
                compressed += huffmanCodes[wc];
            }
        }
    }

    return compressed;
}

// Function to decompress the string using Huffman Tree and common words decoding
std::string decompress(const std::string& compressed) {
    std::string decompressed;
    std::string current;
    for (char c : compressed) {
        if (c == ' ') {
            for (const auto& pair : commonWords) {
                if (pair.second == current) {
                    decompressed += pair.first + " ";
                    current.clear();
                    break;
                }
            }
        } else {
            current += c;
        }
    }
    return decompressed;
}
