#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <vector>
#include <cstdint>

struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int freq) : character(c), frequency(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

void buildTreeStructure(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        structure += '1';
        structure += node->character;
    } else {
        structure += '0';
        buildTreeStructure(node->left, structure);
        buildTreeStructure(node->right, structure);
    }
}

HuffmanNode* reconstructTree(const std::string& data, size_t& pos) {
    if (pos >= data.size()) return nullptr;

    if (data[pos] == '1') {
        pos++;
        HuffmanNode* leaf = new HuffmanNode(data[pos], 0);
        pos++;
        return leaf;
    }

    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = reconstructTree(data, pos);
    node->right = reconstructTree(data, pos);
    return node;
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";

    std::map<char, int> frequencies;
    for (char c : input) {
        frequencies[c]++;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> heap;
    for (const auto& pair : frequencies) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (heap.size() > 1) {
        HuffmanNode* left = heap.top(); heap.pop();
        HuffmanNode* right = heap.top(); heap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        heap.push(parent);
    }
    
    HuffmanNode* root = heap.top();
    std::map<char, std::string> huffmanCodes;
    buildTreeStructure(root, "");
    
    std::string bitString;
    for (char c : input) {
        bitString += huffmanCodes[c];
    }
    
    std::string compressed;
    for (size_t i = 0; i < bitString.size(); i += 8) {
        std::string byteStr = bitString.substr(i, 8);
        if (byteStr.size() < 8) byteStr.append(8 - byteStr.size(), '0');
        std::bitset<8> bits(byteStr);
        compressed.push_back(static_cast<char>(bits.to_ulong()));
    }
    
    freeTree(root);
    return compressed;
}

std::string decompress(const std::string& data) {
    if (data.empty()) return "";
    
    std::string bitString;
    for (char byte : data) {
        std::bitset<8> bits(static_cast<unsigned char>(byte));
        bitString += bits.to_string();
    }
    
    size_t pos = 0;
    HuffmanNode* root = reconstructTree(bitString, pos);
    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->character;
            current = root;
        }
    }
    
    freeTree(root);
    return output;
}
