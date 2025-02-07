#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Huffman Tree Node Structure
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

// Function to Calculate Character Frequency
std::map<char, int> computeFrequency(const std::string& text) {
    std::map<char, int> frequencyTable;
    for (char symbol : text) {
        frequencyTable[symbol]++;
    }
    return frequencyTable;
}

// Recursive Function to Assign Huffman Codes
void assignHuffmanCodes(HuffmanNode* root, const std::string& path, std::unordered_map<char, std::string>& codeBook) {
    if (!root) return;
    if (!root->leftChild && !root->rightChild) {
        codeBook[root->character] = path;
    }
    assignHuffmanCodes(root->leftChild, path + "0", codeBook);
    assignHuffmanCodes(root->rightChild, path + "1", codeBook);
}

// Function to Construct Huffman Tree
HuffmanNode* constructHuffmanTree(const std::map<char, int>& frequencyData) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> priorityQueue;
    for (const auto& entry : frequencyData) {
        priorityQueue.push(new HuffmanNode(entry.first, entry.second));
    }
    while (priorityQueue.size() > 1) {
        HuffmanNode* left = priorityQueue.top(); priorityQueue.pop();
        HuffmanNode* right = priorityQueue.top(); priorityQueue.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->leftChild = left;
        parent->rightChild = right;
        priorityQueue.push(parent);
    }
    return priorityQueue.top();
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
HuffmanNode* deserializeHuffmanTree(const std::string& encodedTree, size_t& position) {
    if (position >= encodedTree.size()) return nullptr;
    if (encodedTree[position] == '1') {
        position++;
        char symbol = static_cast<char>(std::bitset<8>(encodedTree.substr(position, 8)).to_ulong());
        position += 8;
        return new HuffmanNode(symbol, 0);
    }
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = deserializeHuffmanTree(encodedTree, position);
    node->rightChild = deserializeHuffmanTree(encodedTree, position);
    return node;
}

// Function to Encode Text
std::string encodeText(const std::string& input) {
    std::map<char, int> frequencyMap = computeFrequency(input);
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
std::string decodeText(const std::string& compressedData) {
    size_t position = 0;
    HuffmanNode* root = deserializeHuffmanTree(compressedData, position);
    std::string decodedMessage;
    HuffmanNode* currentNode = root;
    while (position < compressedData.size()) {
        currentNode = (compressedData[position] == '0') ? currentNode->leftChild : currentNode->rightChild;
        position++;
        if (!currentNode->leftChild && !currentNode->rightChild) {
            decodedMessage += currentNode->character;
            currentNode = root;
        }
    }
    return decodedMessage;
}
