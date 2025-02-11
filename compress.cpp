#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct TreeNode {
    char symbol;
    int count;
    TreeNode* leftChild;
    TreeNode* rightChild;

    TreeNode(char s, int c) : symbol(s), count(c), leftChild(nullptr), rightChild(nullptr) {}
};

struct NodeComparator {
    bool operator()(TreeNode* lhs, TreeNode* rhs) {
        return lhs->count > rhs->count;
    }
};

TreeNode* constructHuffmanTree(std::unordered_map<char, int>& charFreq) {
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, NodeComparator> pq;
    for (const auto& entry : charFreq) {
        pq.push(new TreeNode(entry.first, entry.second));
    }
    while (pq.size() > 1) {
        TreeNode* left = pq.top(); pq.pop();
        TreeNode* right = pq.top(); pq.pop();
        TreeNode* merged = new TreeNode('\0', left->count + right->count);
        merged->leftChild = left;
        merged->rightChild = right;
        pq.push(merged);
    }
    return pq.top();
}

void assignHuffmanCodes(TreeNode* node, const std::string& path, std::unordered_map<char, std::string>& codeTable) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        codeTable[node->symbol] = path;
    }
    assignHuffmanCodes(node->leftChild, path + "0", codeTable);
    assignHuffmanCodes(node->rightChild, path + "1", codeTable);
}

void buildTreeStructure(TreeNode* node, std::string& structure) {
    if (!node) return;
    structure += node->leftChild || node->rightChild ? "0" : "1";
    if (!node->leftChild && !node->rightChild) {
        structure += node->symbol;
    } else {
        buildTreeStructure(node->leftChild, structure);
        buildTreeStructure(node->rightChild, structure);
    }
}

TreeNode* reconstructTree(const std::string& serialized, size_t& pos) {
    if (pos >= serialized.size()) return nullptr;
    TreeNode* node = new TreeNode('\0', 0);
    if (serialized[pos++] == '1') {
        node->symbol = serialized[pos++];
    } else {
        node->leftChild = reconstructTree(serialized, pos);
        node->rightChild = reconstructTree(serialized, pos);
    }
    return node;
}

void releaseTree(TreeNode* node) {
    if (!node) return;
    releaseTree(node->leftChild);
    releaseTree(node->rightChild);
    delete node;
}

std::string compress(const std::string& inputText) {
    if (inputText.empty()) return "";
    std::unordered_map<char, int> freqMap;
    for (char ch : inputText) freqMap[ch]++;
    TreeNode* root = constructHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffMap;
    assignHuffmanCodes(root, "", huffMap);
    std::string encodedStream;
    for (char ch : inputText) encodedStream += huffMap[ch];
    std::string treeEncoding;
    buildTreeStructure(root, treeEncoding);
    uint32_t treeSize = treeEncoding.size(), bitStreamSize = encodedStream.size();
    std::string binaryData = std::bitset<32>(bitStreamSize).to_string() + encodedStream;
    releaseTree(root);
    return std::bitset<32>(treeSize).to_string() + treeEncoding + binaryData;
}

std::string decompress(const std::string& inputData) {
    if (inputData.size() < 64) return "";
    size_t pos = 0;
    uint32_t treeSize = std::bitset<32>(inputData.substr(pos, 32)).to_ulong(); pos += 32;
    uint32_t bitCount = std::bitset<32>(inputData.substr(pos, 32)).to_ulong(); pos += 32;
    TreeNode* rootNode = reconstructTree(inputData.substr(pos, treeSize), pos);
    std::string bitSequence = inputData.substr(pos, bitCount);
    std::string originalText;
    TreeNode* cursor = rootNode;
    for (char bit : bitSequence) {
        cursor = (bit == '0') ? cursor->leftChild : cursor->rightChild;
        if (!cursor->leftChild && !cursor->rightChild) {
            originalText += cursor->symbol;
            cursor = rootNode;
        }
    }
    releaseTree(rootNode);
    return originalText;
}
