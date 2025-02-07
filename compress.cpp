#include <iostream>
#include <map>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>

struct HuffmanNode {
    char symbol;
    int count;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char ch, int freq) : symbol(ch), count(freq), left(nullptr), right(nullptr) {}
};

struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->count > b->count;
    }
};

std::map<char, int> computeFrequencies(const std::string& input) {
    std::map<char, int> frequency;
    for (char c : input) {
        frequency[c]++;
    }
    return frequency;
}

void generateHuffmanMap(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& mapping) {
    if (!node) return;
    if (!node->left && !node->right) {
        mapping[node->symbol] = path;
    }
    generateHuffmanMap(node->left, path + "0", mapping);
    generateHuffmanMap(node->right, path + "1", mapping);
}

HuffmanNode* buildHuffmanTree(const std::map<char, int>& frequency) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> heap;
    for (const auto& pair : frequency) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }
    while (heap.size() > 1) {
        HuffmanNode* left = heap.top(); heap.pop();
        HuffmanNode* right = heap.top(); heap.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->count + right->count);
        parent->left = left;
        parent->right = right;
        heap.push(parent);
    }
    return heap.top();
}

void serializeHuffmanTree(HuffmanNode* node, std::vector<unsigned char>& encoding) {
    if (!node) return;
    if (!node->left && !node->right) {
        encoding.push_back(1);
        encoding.push_back(static_cast<unsigned char>(node->symbol));
    } else {
        encoding.push_back(0);
        serializeHuffmanTree(node->left, encoding);
        serializeHuffmanTree(node->right, encoding);
    }
}

HuffmanNode* deserializeHuffmanTree(const std::vector<unsigned char>& encoding, size_t& pos) {
    if (pos >= encoding.size()) return nullptr;
    if (encoding[pos] == 1) {
        pos++;
        return new HuffmanNode(static_cast<char>(encoding[pos++]), 0);
    }
    pos++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = deserializeHuffmanTree(encoding, pos);
    node->right = deserializeHuffmanTree(encoding, pos);
    return node;
}

std::string compress(const std::string& input) {
    std::map<char, int> frequencies = computeFrequencies(input);
    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::unordered_map<char, std::string> encodingMap;
    generateHuffmanMap(root, "", encodingMap);
    std::vector<unsigned char> treeEncoding;
    serializeHuffmanTree(root, treeEncoding);
    std::string bitstream;
    unsigned char byteBuffer = 0;
    int bitCount = 0;
    for (char c : input) {
        std::string code = encodingMap[c];
        for (char bit : code) {
            byteBuffer <<= 1;
            if (bit == '1') byteBuffer |= 1;
            bitCount++;
            if (bitCount == 8) {
                bitstream.push_back(byteBuffer);
                byteBuffer = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        byteBuffer <<= (8 - bitCount);
        bitstream.push_back(byteBuffer);
    }
    std::string compressedData(treeEncoding.begin(), treeEncoding.end());
    compressedData += bitstream;
    return compressedData;
}

std::string decompress(const std::string& compressedInput) {
    size_t pos = 0;
    std::vector<unsigned char> encoding(compressedInput.begin(), compressedInput.end());
    HuffmanNode* root = deserializeHuffmanTree(encoding, pos);
    std::string output;
    HuffmanNode* current = root;
    while (pos < encoding.size()) {
        unsigned char byte = encoding[pos];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                output += current->symbol;
                current = root;
            }
        }
        pos++;
    }
    return output;
}
