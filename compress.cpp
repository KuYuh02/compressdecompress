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

HuffmanNode* constructHuffmanTree(std::map<char, int>& frequencies) {
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
    
    return heap.top();
}

void generateHuffmanCodes(HuffmanNode* node, const std::string& path, std::map<char, std::string>& codes) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        codes[node->character] = path;
    }
    
    generateHuffmanCodes(node->left, path + "0", codes);
    generateHuffmanCodes(node->right, path + "1", codes);
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

std::string binaryEncode(const std::string& bits) {
    std::stringstream stream;
    for (size_t i = 0; i < bits.size(); i += 8) {
        std::string chunk = bits.substr(i, 8);
        if (chunk.size() < 8) {
            chunk.append(8 - chunk.size(), '0');
        }
        std::bitset<8> byte(chunk);
        stream.put(static_cast<char>(byte.to_ulong()));
    }
    return stream.str();
}

std::string binaryDecode(const std::string& data) {
    std::string bits;
    for (char byte : data) {
        std::bitset<8> bitset(static_cast<unsigned char>(byte));
        bits += bitset.to_string();
    }
    return bits;
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
    
    std::string binaryCompressed = binaryEncode(encodedBits);

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

std::string decompress(const std::string& data) {
    if (data.size() < 8) return "";
    
    const unsigned char* rawData = reinterpret_cast<const unsigned char*>(data.data());
    uint32_t treeSize = (rawData[0] << 24) | (rawData[1] << 16) | (rawData[2] << 8) | rawData[3];
    uint32_t bitLength = (rawData[4] << 24) | (rawData[5] << 16) | (rawData[6] << 8) | rawData[7];

    size_t treeStart = 8;
    size_t treeEnd = treeStart + treeSize;
    if (data.size() < treeEnd) return "";

    std::string treeData = data.substr(treeStart, treeSize);
    std::string encodedData = data.substr(treeEnd);
    
    size_t pos = 0;
    HuffmanNode* root = reconstructTree(treeData, pos);
    
    std::string bitString = binaryDecode(encodedData);
    if (bitString.size() > bitLength) {
        bitString.resize(bitLength);
    }
    
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
