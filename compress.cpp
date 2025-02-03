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
    bool operator>(const Node& other) const {
        return freq > other.freq;
    }
};

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

    // Create a node for each character and add it to the priority queue
    for (const auto& pair : frequencyMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    // Build the Huffman Tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top();
        minHeap.pop();
        Node* right = minHeap.top();
        minHeap.pop();

        Node* internalNode = new Node('\0', left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;
        minHeap.push(internalNode);
    }

    return minHeap.top();  // Root of the Huffman Tree
}

// Function to compress the string using Huffman codes
std::string compress(const std::string& input) {
    // Step 1: Build the frequency map
    std::map<char, int> frequencyMap = buildFrequencyMap(input);

    // Step 2: Build the Huffman Tree
    Node* root = buildHuffmanTree(frequencyMap);

    // Step 3: Generate Huffman Codes
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Step 4: Compress the input string using Huffman Codes (binary form)
    std::string compressed;
    for (char c : input) {
        compressed += huffmanCodes[c];  // Append the Huffman code for each character
    }

    delete root; // Free memory
    return compressed;
}

// Function to decompress the string using the Huffman Tree
std::string decompress(const std::string& compressed) {
    size_t index = 0;
    // Step 1: Deserialize the Huffman tree from the compressed string
    Node* root = buildHuffmanTree(buildFrequencyMap(compressed));

    // Step 2: Decompress the compressed string using the Huffman Tree
    std::string decompressed;
    Node* current = root;
    while (index < compressed.size()) {
        if (compressed[index] == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        index++;

        // If we've reached a leaf node, add the character to the decompressed string
        if (!current->left && !current->right) {
            decompressed += current->data;
            current = root;  // Reset to root for the next character
        }
    }
    
    delete root; // Free memory
    return decompressed;
}
