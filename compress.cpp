#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

struct Node { // huffman node
    char character;
    int frequency;
    Node* left;
    Node* right;

    Node(char c, int freq) : character(c), frequency(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};

void serializeTree(Node* root, std::string& treeStructure) {
    if (!root) return;

    if (!root->left && !root->right) { 
        treeStructure += "1";  // leaf node
        treeStructure += root->character; 
    } else {
        treeStructure += "0";  // internal node
        serializeTree(root->left, treeStructure);
        serializeTree(root->right, treeStructure);
    }
}

Node* deserializeTree(const std::string& treeData, size_t& index) {
    if (index >= treeData.size()) return nullptr;

    if (treeData[index] == '1') {  // leaf node
        index++;
        Node* leaf = new Node(treeData[index], 0);
        index++;
        return leaf;
    }

    index++;
    Node* node = new Node('\0', 0);  // internal node
    node->left = deserializeTree(treeData, index);
    node->right = deserializeTree(treeData, index);
    return node;
}

Node* huffmanTreeBuilder(std::map<char, int>& frequency) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;

    for (const auto& pair : frequency) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top();  // save first smallest
        minHeap.pop();               // pop
        Node* right = minHeap.top(); // save second smallest
        minHeap.pop();               // pop

        // combine two smallest
        Node* node = new Node('\0', left->frequency + right->frequency);
        node->left = left;
        node->right = right;
        minHeap.push(node);
    }

    return minHeap.top();
}

void HuffmanCoder(Node* root, const std::string& code, std::map<char, std::string>& huffmanCodes) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->character] = code;
    }

    HuffmanCoder(root->left, code + "0", huffmanCodes);
    HuffmanCoder(root->right, code + "1", huffmanCodes);
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

std::string encodeToBinary(const std::string& bitString) {
    std::stringstream bitStream;
    for (size_t i = 0; i < bitString.size(); i += 8) {
        std::string byteStr = bitString.substr(i, 8);
        if (byteStr.size() < 8) {
            byteStr.append(8 - byteStr.size(), '0');
        }
        std::bitset<8> bits(byteStr);
        bitStream.put(static_cast<char>(bits.to_ulong()));
    }
    return bitStream.str();
}

std::string decodeFromBinary(const std::string& binaryData) {
    std::string bitString;
    for (char c : binaryData) {
        std::bitset<8> bits(static_cast<unsigned char>(c));
        bitString += bits.to_string();
    }
    return bitString;
}

std::string compress(const std::string& source) {
    if (source.empty()) {
        return "";
    }

    std::map<char, int> letterFreq;
    for (char c : source) {
        letterFreq[c]++;
    }

    Node* tree = huffmanTreeBuilder(letterFreq);
    std::map<char, std::string> huffmanCodes;
    HuffmanCoder(tree, "", huffmanCodes);

    std::string compressedBits;
    for (char c : source) {
        compressedBits += huffmanCodes[c];
    }

    std::string treeData;
    serializeTree(tree, treeData);
    uint32_t treeSize = treeData.size();
    uint32_t bitLength = compressedBits.size();

    std::string compressedBinary = encodeToBinary(compressedBits);

    std::string header(8, '\0');
    header[0] = (treeSize >> 24) & 0xFF;
    header[1] = (treeSize >> 16) & 0xFF;
    header[2] = (treeSize >> 8) & 0xFF;
    header[3] = treeSize & 0xFF;
    header[4] = (bitLength >> 24) & 0xFF;
    header[5] = (bitLength >> 16) & 0xFF;
    header[6] = (bitLength >> 8) & 0xFF;
    header[7] = bitLength & 0xFF;

    deleteTree(tree);

    return header + treeData + compressedBinary;
}

std::string decompress(const std::string& source) {
    if (source.size() < 8) {
        return "";
    }

    const unsigned char* data = reinterpret_cast<const unsigned char*>(source.data());
    uint32_t treeSize = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    uint32_t bitLength = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

    size_t headerSize = 8;
    size_t treeDataEnd = headerSize + treeSize;

    if (source.size() < treeDataEnd) {
        return ""; 
    }

    std::string treeData = source.substr(headerSize, treeSize);
    std::string compressedBinary = source.substr(treeDataEnd);

    size_t index = 0;
    Node* treeRoot = deserializeTree(treeData, index);

    std::string bitString = decodeFromBinary(compressedBinary);
    if (bitString.size() > bitLength) {
        bitString.resize(bitLength);
    } else if (bitString.size() < bitLength) {
        deleteTree(treeRoot);
        return ""; 
    }

    std::string decompressed;
    Node* current = treeRoot;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;

        if (!current->left && !current->right) {
            decompressed += current->character;
            current = treeRoot;
        }
    }

    deleteTree(treeRoot);
    return decompressed;
}
