#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <bitset>

// Structure representing a node in the Huffman Tree
struct TreeNode {
    char value;
    int count;
    TreeNode* left;
    TreeNode* right;

    TreeNode(char v, int c) : value(v), count(c), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(TreeNode* a, TreeNode* b) {
        return a->count > b->count;
    }
};

// Function to calculate character frequency
std::map<char, int> getFrequency(const std::string& text) {
    std::map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }
    return freq;
}

// Function to generate Huffman codes
void createHuffmanCodes(TreeNode* node, const std::string& path, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        codes[node->value] = path;
    }
    createHuffmanCodes(node->left, path + "0", codes);
    createHuffmanCodes(node->right, path + "1", codes);
}

// Function to build Huffman Tree
TreeNode* buildHuffmanTree(const std::map<char, int>& frequencies) {
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, CompareNodes> heap;
    for (const auto& pair : frequencies) {
        heap.push(new TreeNode(pair.first, pair.second));
    }
    while (heap.size() > 1) {
        TreeNode* left = heap.top(); heap.pop();
        TreeNode* right = heap.top(); heap.pop();
        TreeNode* parent = new TreeNode('\0', left->count + right->count);
        parent->left = left;
        parent->right = right;
        heap.push(parent);
    }
    return heap.top();
}

// Function to serialize the Huffman tree
std::string encodeTree(TreeNode* root) {
    if (!root) return "";
    if (!root->left && !root->right) {
        return "1" + std::bitset<8>(root->value).to_string();
    }
    return "0" + encodeTree(root->left) + encodeTree(root->right);
}

// Function to deserialize the Huffman tree
TreeNode* decodeTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;
    if (data[pos] == '1') {
        pos++;
        char ch = static_cast<char>(std::bitset<8>(data.substr(pos, 8)).to_ulong());
        pos += 8;
        return new TreeNode(ch, 0);
    }
    pos++;
    TreeNode* node = new TreeNode('\0', 0);
    node->left = decodeTree(data, pos);
    node->right = decodeTree(data, pos);
    return node;
}

// Function to compress a string
std::string compressString(const std::string& input) {
    std::map<char, int> freqMap = getFrequency(input);
    TreeNode* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffmanCode;
    createHuffmanCodes(root, "", huffmanCode);
    std::string serializedTree = encodeTree(root);
    std::string compressed;
    for (char c : input) {
        compressed += huffmanCode[c];
    }
    return serializedTree + compressed;
}

// Function to decompress a string
std::string decompressString(const std::string& encoded) {
    size_t pos = 0;
    TreeNode* root = decodeTree(encoded, pos);
    std::string decodedText;
    TreeNode* current = root;
    while (pos < encoded.size()) {
        current = (encoded[pos] == '0') ? current->left : current->right;
        pos++;
        if (!current->left && !current->right) {
            decodedText += current->value;
            current = root;
        }
    }
    return decodedText;
}
