#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Node structure for Huffman Tree
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Function to build frequency map
std::map<char, int> buildFrequencyMap(const std::string& input) {
    std::map<char, int> frequency;
    for (char ch : input) {
        frequency[ch]++;
    }
    return frequency;
}

// Function to generate Huffman codes
void generateCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;
    if (!root->left && !root->right) {
        codeMap[root->data] = code;
    }
    generateCodes(root->left, code + "0", codeMap);
    generateCodes(root->right, code + "1", codeMap);
}

// Function to build Huffman Tree
Node* buildTree(const std::map<char, int>& freqMap) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    for (const auto& pair : freqMap) {
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

// Function to serialize the Huffman tree
std::string serializeTree(Node* root) {
    if (!root) return "";
    if (!root->left && !root->right) {
        return "1" + std::bitset<8>(root->data).to_string();
    }
    return "0" + serializeTree(root->left) + serializeTree(root->right);
}

// Function to deserialize the Huffman tree
Node* deserializeTree(const std::string& data, size_t& index) {
    if (index >= data.size()) return nullptr;
    if (data[index] == '1') {
        index++;
        char ch = static_cast<char>(std::bitset<8>(data.substr(index, 8)).to_ulong());
        index += 8;
        return new Node(ch, 0);
    }
    index++;
    Node* node = new Node('\0', 0);
    node->left = deserializeTree(data, index);
    node->right = deserializeTree(data, index);
    return node;
}

// Function to compress a string
std::string compress(const std::string& input) {
    std::map<char, int> freqMap = buildFrequencyMap(input);
    Node* root = buildTree(freqMap);
    std::unordered_map<char, std::string> codes;
    generateCodes(root, "", codes);
    std::string treeEncoding = serializeTree(root);
    std::string compressedData;
    for (char ch : input) {
        compressedData += codes[ch];
    }
    return treeEncoding + compressedData;
}

// Function to decompress a string
std::string decompress(const std::string& encodedData) {
    size_t index = 0;
    Node* root = deserializeTree(encodedData, index);
    std::string decodedText;
    Node* current = root;
    while (index < encodedData.size()) {
        current = (encodedData[index] == '0') ? current->left : current->right;
        index++;
        if (!current->left && !current->right) {
            decodedText += current->data;
            current = root;
        }
    }
    return decodedText;
}
