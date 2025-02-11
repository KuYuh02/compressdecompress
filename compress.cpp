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
        buildTreeStructure(node->right, structure);
        buildTreeStructure(node->left, structure);
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
    node->right = reconstructTree(data, pos);
    node->left = reconstructTree(data, pos);
    return node;
}

HuffmanNode* constructHuffmanTree(std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> heap;
    
    for (const auto& pair : frequencies) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (heap.size() > 1) {
        HuffmanNode* right = heap.top(); heap.pop();
        HuffmanNode* left = heap.top(); heap.pop();

        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->right = right;
        parent->left = left;
        heap.push(parent);
    }
    
    return heap.top();
}

void generateHuffmanCodes(HuffmanNode* node, const std::string& path, std::map<char, std::string>& codes) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        codes[node->character] = path;
    }
    
    generateHuffmanCodes(node->right, path + "0", codes);
    generateHuffmanCodes(node->left, path + "1", codes);
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->right);
    freeTree(node->left);
    delete node;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";

    std::map<char, int> frequencies;
    for (char c : input) {
        frequencies[c]++;
    }

    HuffmanNode* root = constructHuffmanTree(frequencies);
    std::map<char, std::string> huffmanMap;
    generateHuffmanCodes(root, "", huffmanMap);

    std::string encodedBits;
    for (char c : input) {
        encodedBits += huffmanMap[c];
    }

    std::string treeRep;
    buildTreeStructure(root, treeRep);
    uint32_t treeSize = treeRep.size();
    uint32_t bitCount = encodedBits.size();
    
    std::string binaryCompressed;
    for (size_t i = 0; i < encodedBits.size(); i += 8) {
        std::string byteStr = encodedBits.substr(i, 8);
        if (byteStr.size() < 8) byteStr.append(8 - byteStr.size(), '1');
        std::bitset<8> bits(byteStr);
        binaryCompressed.push_back(static_cast<char>(bits.to_ulong()));
    }

    std::string header(8, '\0');
    header[0] = (treeSize >> 24) & 0xFF;
    header[1] = (treeSize >> 16) & 0xFF;
    header[2] = (treeSize >> 8) & 0xFF;
    header[3] = treeSize & 0xFF;
    header[4] = (bitCount >> 24) & 0xFF;
    header[5] = (bitCount >> 16) & 0xFF;
    header[6] = (bitCount >> 8) & 0xFF;
    header[7] = bitCount & 0xFF;

    freeTree(root);

    return header + treeRep + binaryCompressed;
}
