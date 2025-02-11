#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void encodeTree(HuffmanNode* root, std::string& structure) {
    if (!root) return;

    if (!root->left && !root->right) {
        structure += "1";
        structure += root->ch;
    } else {
        structure += "0";
        encodeTree(root->left, structure);
        encodeTree(root->right, structure);
    }
}

HuffmanNode* decodeTree(const std::string& data, size_t& idx) {
    if (idx >= data.size()) return nullptr;

    if (data[idx] == '1') {
        idx++;
        HuffmanNode* node = new HuffmanNode(data[idx], 0);
        idx++;
        return node;
    }

    idx++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(data, idx);
    node->right = decodeTree(data, idx);
    return node;
}

HuffmanNode* buildHuffmanTree(std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;

    for (const auto& entry : freqMap) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}

void generateHuffmanCodes(HuffmanNode* root, const std::string& path, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;
    if (!root->left && !root->right) {
        codeMap[root->ch] = path;
    }
    generateHuffmanCodes(root->left, path + "0", codeMap);
    generateHuffmanCodes(root->right, path + "1", codeMap);
}

void freeTree(HuffmanNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

std::string binaryToByteString(const std::string& bits) {
    std::stringstream output;
    for (size_t i = 0; i < bits.size(); i += 8) {
        std::string chunk = bits.substr(i, 8);
        while (chunk.size() < 8) chunk += '0';
        output.put(static_cast<char>(std::bitset<8>(chunk).to_ulong()));
    }
    return output.str();
}

std::string byteStringToBinary(const std::string& data) {
    std::string bitString;
    for (char byte : data) {
        bitString += std::bitset<8>(static_cast<unsigned char>(byte)).to_string();
    }
    return bitString;
}

std::string compress(const std::string& input) {
    if (input.empty()) return "";

    std::unordered_map<char, int> frequencies;
    for (char c : input) {
        frequencies[c]++;
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::unordered_map<char, std::string> encodingMap;
    generateHuffmanCodes(root, "", encodingMap);

    std::string encodedBits;
    for (char c : input) {
        encodedBits += encodingMap[c];
    }

    std::string treeStructure;
    encodeTree(root, treeStructure);
    uint32_t treeSize = treeStructure.size();
    uint32_t bitSize = encodedBits.size();
    std::string compressedData = binaryToByteString(encodedBits);

    std::string header(8, '\0');
    header[0] = (treeSize >> 24) & 0xFF;
    header[1] = (treeSize >> 16) & 0xFF;
    header[2] = (treeSize >> 8) & 0xFF;
    header[3] = treeSize & 0xFF;
    header[4] = (bitSize >> 24) & 0xFF;
    header[5] = (bitSize >> 16) & 0xFF;
    header[6] = (bitSize >> 8) & 0xFF;
    header[7] = bitSize & 0xFF;

    freeTree(root);
    return header + treeStructure + compressedData;
}

std::string decompress(const std::string& data) {
    if (data.size() < 8) return "";

    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data.data());
    uint32_t treeLen = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    uint32_t bitLen = (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];

    size_t treeStart = 8;
    size_t treeEnd = treeStart + treeLen;
    if (data.size() < treeEnd) return "";

    std::string treeData = data.substr(treeStart, treeLen);
    std::string encodedData = data.substr(treeEnd);
    size_t pos = 0;
    HuffmanNode* root = decodeTree(treeData, pos);

    std::string bitStream = byteStringToBinary(encodedData);
    if (bitStream.size() > bitLen) {
        bitStream.resize(bitLen);
    }

    std::string output;
    HuffmanNode* current = root;
    for (char bit : bitStream) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output += current->ch;
            current = root;
        }
    }

    freeTree(root);
    return output;
}
