#include <iostream>
#include <map>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>

class HuffmanNode {
public:
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

std::map<char, int> calculateFrequencies(const std::string& text) {
    std::map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }
    return freqMap;
}

void buildEncodingMap(HuffmanNode* node, const std::string& path, std::unordered_map<char, std::string>& encodingMap) {
    if (!node) return;
    if (!node->left && !node->right) {
        encodingMap[node->character] = path;
    }
    buildEncodingMap(node->left, path + "0", encodingMap);
    buildEncodingMap(node->right, path + "1", encodingMap);
}

HuffmanNode* createHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

void encodeTree(HuffmanNode* node, std::vector<unsigned char>& treeData) {
    if (!node) return;
    if (!node->left && !node->right) {
        treeData.push_back(1);
        treeData.push_back(static_cast<unsigned char>(node->character));
    } else {
        treeData.push_back(0);
        encodeTree(node->left, treeData);
        encodeTree(node->right, treeData);
    }
}

HuffmanNode* decodeTree(const std::vector<unsigned char>& treeData, size_t& index) {
    if (index >= treeData.size()) return nullptr;
    if (treeData[index] == 1) {
        index++;
        return new HuffmanNode(static_cast<char>(treeData[index++]), 0);
    }
    index++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = decodeTree(treeData, index);
    node->right = decodeTree(treeData, index);
    return node;
}

std::string compress(const std::string& input) {
    std::map<char, int> frequencyMap = calculateFrequencies(input);
    HuffmanNode* root = createHuffmanTree(frequencyMap);
    std::unordered_map<char, std::string> encodingMap;
    buildEncodingMap(root, "", encodingMap);
    std::vector<unsigned char> treeEncoding;
    encodeTree(root, treeEncoding);
    std::string compressedData;
    unsigned char byteBuffer = 0;
    int bitCounter = 0;
    for (char c : input) {
        std::string code = encodingMap[c];
        for (char bit : code) {
            byteBuffer <<= 1;
            if (bit == '1') byteBuffer |= 1;
            bitCounter++;
            if (bitCounter == 8) {
                compressedData.push_back(byteBuffer);
                byteBuffer = 0;
                bitCounter = 0;
            }
        }
    }
    if (bitCounter > 0) {
        byteBuffer <<= (8 - bitCounter);
        compressedData.push_back(byteBuffer);
    }
    std::string finalOutput(treeEncoding.begin(), treeEncoding.end());
    finalOutput += compressedData;
    return finalOutput;
}

std::string decompress(const std::string& compressedInput) {
    size_t index = 0;
    std::vector<unsigned char> treeData(compressedInput.begin(), compressedInput.end());
    HuffmanNode* root = decodeTree(treeData, index);
    std::string decompressedOutput;
    HuffmanNode* currentNode = root;
    while (index < treeData.size()) {
        unsigned char byte = treeData[index];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            currentNode = bit ? currentNode->right : currentNode->left;
            if (!currentNode->left && !currentNode->right) {
                decompressedOutput += currentNode->character;
                currentNode = root;
            }
        }
        index++;
    }
    return decompressedOutput;
}
