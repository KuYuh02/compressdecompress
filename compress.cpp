#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct NodeComparator {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void encodeTree(HuffmanNode* node, std::string& structure) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        structure += "1";
        structure += node->ch;
    } else {
        structure += "0";
        encodeTree(node->left, structure);
        encodeTree(node->right, structure);
    }
}

HuffmanNode* decodeTree(const std::string& data, size_t& idx) {
    if (idx >= data.size()) return nullptr;

    if (data[idx] == '1') {
        idx++;
        HuffmanNode* leaf = new HuffmanNode(data[idx], 0);
        idx++;
        return leaf;
    }

    idx++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(data, idx);
    node->right = decodeTree(data, idx);
    return node;
}

HuffmanNode* constructTree(std::map<char, int>& freqTable) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;

    for (const auto& pair : freqTable) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        minHeap.push(merged);
    }
    return minHeap.top();
}

void generateCodes(HuffmanNode* root, const std::string& code, std::map<char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = code;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

void deleteHuffmanTree(HuffmanNode* root) {
    if (!root) return;
    deleteHuffmanTree(root->left);
    deleteHuffmanTree(root->right);
    delete root;
}

std::string binaryEncode(const std::string& bitData) {
    std::stringstream stream;
    for (size_t i = 0; i < bitData.size(); i += 8) {
        std::string chunk = bitData.substr(i, 8);
        if (chunk.size() < 8) chunk.append(8 - chunk.size(), '0');
        std::bitset<8> bits(chunk);
        stream.put(static_cast<char>(bits.to_ulong()));
    }
    return stream.str();
}

std::string binaryDecode(const std::string& binData) {
    std::string bitStr;
    for (char c : binData) {
        std::bitset<8> bits(static_cast<unsigned char>(c));
        bitStr += bits.to_string();
    }
    return bitStr;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";
    
    std::map<char, int> freqMap;
    for (char c : input) freqMap[c]++;
    
    HuffmanNode* treeRoot = constructTree(freqMap);
    std::map<char, std::string> huffmanCode;
    generateCodes(treeRoot, "", huffmanCode);

    std::string encodedBits;
    for (char c : input) encodedBits += huffmanCode[c];
    
    std::string treeStructure;
    encodeTree(treeRoot, treeStructure);
    uint32_t treeLen = treeStructure.size();
    uint32_t bitCount = encodedBits.size();
    
    std::string binaryCompressed = binaryEncode(encodedBits);
    std::string header(8, '\0');
    header[0] = (treeLen >> 24) & 0xFF;
    header[1] = (treeLen >> 16) & 0xFF;
    header[2] = (treeLen >> 8) & 0xFF;
    header[3] = treeLen & 0xFF;
    header[4] = (bitCount >> 24) & 0xFF;
    header[5] = (bitCount >> 16) & 0xFF;
    header[6] = (bitCount >> 8) & 0xFF;
    header[7] = bitCount & 0xFF;
    
    deleteHuffmanTree(treeRoot);
    return header + treeStructure + binaryCompressed;
}

std::string decompress(const std::string& input) {
    if (input.size() < 8) return "";
    
    const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
    uint32_t treeLen = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    uint32_t bitLen = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    
    size_t treeDataEnd = 8 + treeLen;
    if (input.size() < treeDataEnd) return "";
    
    std::string treeData = input.substr(8, treeLen);
    std::string binaryData = input.substr(treeDataEnd);
    
    size_t index = 0;
    HuffmanNode* treeRoot = decodeTree(treeData, index);
    
    std::string bitStream = binaryDecode(binaryData);
    if (bitStream.size() > bitLen) bitStream.resize(bitLen);
    
    std::string output;
    HuffmanNode* current = treeRoot;
    for (char bit : bitStream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->ch;
            current = treeRoot;
        }
    }
    
    deleteHuffmanTree(treeRoot);
    return output;
}
