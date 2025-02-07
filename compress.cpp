#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Definition of a Huffman Tree node
struct TreeNode {
    char symbol;
    int weight;
    TreeNode* leftBranch;
    TreeNode* rightBranch;

    TreeNode(char sym, int wt) : symbol(sym), weight(wt), leftBranch(nullptr), rightBranch(nullptr) {}
};

// Custom comparator for priority queue
struct NodeComparator {
    bool operator()(TreeNode* a, TreeNode* b) {
        return a->weight > b->weight;
    }
};

// Function to determine frequency of characters
std::map<char, int> calculateFrequency(const std::string& text) {
    std::map<char, int> frequencyTable;
    for (char character : text) {
        frequencyTable[character]++;
    }
    return frequencyTable;
}

// Function to assign Huffman codes
void generateCodes(TreeNode* node, const std::string& path, std::unordered_map<char, std::string>& codeMap) {
    if (!node) return;
    
    if (!node->leftBranch && !node->rightBranch) {
        codeMap[node->symbol] = path;
    }
    
    generateCodes(node->leftBranch, path + "0", codeMap);
    generateCodes(node->rightBranch, path + "1", codeMap);
}

// Function to construct Huffman Tree
TreeNode* buildHuffmanTree(const std::map<char, int>& frequencyData) {
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, NodeComparator> priorityQueue;
    
    for (const auto& entry : frequencyData) {
        priorityQueue.push(new TreeNode(entry.first, entry.second));
    }
    
    while (priorityQueue.size() > 1) {
        TreeNode* leftNode = priorityQueue.top(); priorityQueue.pop();
        TreeNode* rightNode = priorityQueue.top(); priorityQueue.pop();
        TreeNode* parentNode = new TreeNode('\0', leftNode->weight + rightNode->weight);
        parentNode->leftBranch = leftNode;
        parentNode->rightBranch = rightNode;
        priorityQueue.push(parentNode);
    }
    
    return priorityQueue.top();
}

// Function to serialize Huffman Tree
std::string serializeHuffmanTree(TreeNode* root) {
    if (!root) return "";
    if (!root->leftBranch && !root->rightBranch) {
        return "1" + std::bitset<8>(root->symbol).to_string();
    }
    return "0" + serializeHuffmanTree(root->leftBranch) + serializeHuffmanTree(root->rightBranch);
}

// Function to deserialize Huffman Tree
TreeNode* deserializeHuffmanTree(const std::string& data, size_t& position) {
    if (position >= data.size()) return nullptr;
    
    if (data[position] == '1') {
        position++;
        char sym = static_cast<char>(std::bitset<8>(data.substr(position, 8)).to_ulong());
        position += 8;
        return new TreeNode(sym, 0);
    }
    
    position++;
    TreeNode* node = new TreeNode('\0', 0);
    node->leftBranch = deserializeHuffmanTree(data, position);
    node->rightBranch = deserializeHuffmanTree(data, position);
    return node;
}

// Function to encode a given text
std::string encodeText(const std::string& input) {
    std::map<char, int> frequencyMap = calculateFrequency(input);
    TreeNode* root = buildHuffmanTree(frequencyMap);
    
    std::unordered_map<char, std::string> huffmanCodeMap;
    generateCodes(root, "", huffmanCodeMap);
    
    std::string treeEncoding = serializeHuffmanTree(root);
    std::string encodedResult;
    for (char character : input) {
        encodedResult += huffmanCodeMap[character];
    }
    
    return treeEncoding + encodedResult;
}

// Function to decode an encoded text
std::string decodeText(const std::string& encodedData) {
    size_t position = 0;
    TreeNode* root = deserializeHuffmanTree(encodedData, position);
    
    std::string decodedMessage;
    TreeNode* currentNode = root;
    while (position < encodedData.size()) {
        currentNode = (encodedData[position] == '0') ? currentNode->leftBranch : currentNode->rightBranch;
        position++;
        
        if (!currentNode->leftBranch && !currentNode->rightBranch) {
            decodedMessage += currentNode->symbol;
            currentNode = root;
        }
    }
    
    return decodedMessage;
}
