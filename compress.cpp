#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Node structure for Huffman Tree
struct TreeNode {
    char symbol;
    int weight;
    TreeNode* left;
    TreeNode* right;

    TreeNode(char s, int w) : symbol(s), weight(w), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct NodeComparer {
    bool operator()(TreeNode* a, TreeNode* b) {
        return a->weight > b->weight;
    }
};

// Function to build frequency table
std::map<char, int> calculateFrequencies(const std::string& text) {
    std::map<char, int> freqTable;
    for (char ch : text) {
        freqTable[ch]++;
    }
    return freqTable;
}

// Function to generate Huffman codes
void generateHuffmanCodes(TreeNode* node, const std::string& path, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        codes[node->symbol] = path;
    }
    generateHuffmanCodes(node->left, path + "0", codes);
    generateHuffmanCodes(node->right, path + "1", codes);
}

// Function to construct Huffman Tree
TreeNode* constructTree(const std::map<char, int>& freqTable) {
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, NodeComparer> pq;
    for (const auto& pair : freqTable) {
        pq.push(new TreeNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        TreeNode* left = pq.top(); pq.pop();
        TreeNode* right = pq.top(); pq.pop();
        TreeNode* parent = new TreeNode('\0', left->weight + right->weight);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

// Function to serialize Huffman tree
std::string serializeTree(TreeNode* root) {
    if (!root) return "";
    if (!root->left && !root->right) {
        return "1" + std::bitset<8>(root->symbol).to_string();
    }
    return "0" + serializeTree(root->left) + serializeTree(root->right);
}

// Function to deserialize Huffman tree
TreeNode* deserializeTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;
    if (data[pos] == '1') {
        pos++;
        char sym = static_cast<char>(std::bitset<8>(data.substr(pos, 8)).to_ulong());
        pos += 8;
        return new TreeNode(sym, 0);
    }
    pos++;
    TreeNode* node = new TreeNode('\0', 0);
    node->left = deserializeTree(data, pos);
    node->right = deserializeTree(data, pos);
    return node;
}

// Function to compress a string
std::string compressData(const std::string& input) {
    std::map<char, int> freqTable = calculateFrequencies(input);
    TreeNode* root = constructTree(freqTable);
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    std::string treeStructure = serializeTree(root);
    std::string compressed;
    for (char ch : input) {
        compressed += huffmanCodes[ch];
    }
    return treeStructure + compressed;
}

// Function to decompress a string
std::string decompressData(const std::string& encodedText) {
    size_t pos = 0;
    TreeNode* root = deserializeTree(encodedText, pos);
    std::string decoded;
    TreeNode* current = root;
    while (pos < encodedText.size()) {
        current = (encodedText[pos] == '0') ? current->left : current->right;
        pos++;
        if (!current->left && !current->right) {
            decoded += current->symbol;
            current = root;
        }
    }
    return decoded;
}
