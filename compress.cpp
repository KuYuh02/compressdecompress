#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Huffman Tree Node
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), leftChild(nullptr), rightChild(nullptr) {}
};

// Comparator for Priority Queue
struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to Create Frequency Map
std::map<char, int> calculateFrequency(const std::string& text) {
    std::map<char, int> frequencyTable;
    for (char symbol : text) {
        frequencyTable[symbol]++;
    }
    return frequencyTable;
}

// Function to Assign Huffman Codes
void assignHuffmanCodes(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& codeDictionary) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        codeDictionary[node->character] = path;
    }
    assignHuffmanCodes(node->leftChild, path + "0", codeDictionary);
    assignHuffmanCodes(node->rightChild, path + "1", codeDictionary);
}

// Function to Construct Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& frequencyData) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (const auto& entry : frequencyData) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->leftChild = left;
        parent->rightChild = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}

// Function to Serialize Huffman Tree
std::string serializeHuffmanTree(HuffmanNode* root) {
    if (!root) return "";
    if (!root->leftChild && !root->rightChild) {
        return "1" + std::bitset<8>(root->character).to_string();
    }
    return "0" + serializeHuffmanTree(root->leftChild) + serializeHuffmanTree(root->rightChild);
}

// Function to Deserialize Huffman Tree
HuffmanNode* deserializeHuffmanTree(const std::string& data, size_t& position) {
    if (position >= data.size()) return nullptr;
    if (data[position] == '1') {
        position++;
        char symbol = static_cast<char>(std::bitset<8>(data.substr(position, 8)).to_ulong());
        position += 8;
        return new HuffmanNode(symbol, 0);
    }
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeHuffmanTree(data, position);
    node->rightChild = deserializeHuffmanTree(data, position);
    return node;
}

// Function to Encode Text
std::string encodeText(const std::string& input) {
    std::map<char, int> frequencyMap = calculateFrequency(input);
    HuffmanNode* root = constructHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> huffmanCodes;
    assignHuffmanCodes(root, "", huffmanCodes);
    std::string serializedTree = serializeHuffmanTree(root);
    std::string encodedData;
    for (char character : input) {
        encodedData += huffmanCodes[character];
    }
    return serializedTree + encodedData;
}

// Function to Decode Text
std::string decodeText(const std::string& encodedData) {
    size_t position = 0;
    HuffmanNode* root = deserializeHuffmanTree(encodedData, position);
    std::string decodedMessage;
    HuffmanNode* currentNode = root;
    while (position < encodedData.size()) {
        currentNode = (encodedData[position] == '0') ? currentNode->leftChild : currentNode->rightChild;
        position++;
        if (!currentNode->leftChild && !currentNode->rightChild) {
            decodedMessage += currentNode->character;
            currentNode = root;
        }
    }
    return decodedMessage;
}
