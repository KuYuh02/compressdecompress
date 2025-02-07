#include <iostream>
#include <map>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>

struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), leftChild(nullptr), rightChild(nullptr) {}
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

void createHuffmanCodes(HuffmanNode* node, const std::string& code, std::unordered_map<char, std::string>& codeMap) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        codeMap[node->character] = code;
    }
    createHuffmanCodes(node->leftChild, code + "0", codeMap);
    createHuffmanCodes(node->rightChild, code + "1", codeMap);
}

HuffmanNode* constructHuffmanTree(const std::map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;
    for (const auto& entry : freqMap) {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* parentNode = new HuffmanNode('\0', left->frequency + right->frequency);
        parentNode->leftChild = left;
        parentNode->rightChild = right;
        minHeap.push(parentNode);
    }
    return minHeap.top();
}

void encodeTree(HuffmanNode* node, std::vector<unsigned char>& treeEncoding) {
    if (!node) return;
    if (!node->leftChild && !node->rightChild) {
        treeEncoding.push_back(1);
        treeEncoding.push_back(static_cast<unsigned char>(node->character));
    } else {
        treeEncoding.push_back(0);
        encodeTree(node->leftChild, treeEncoding);
        encodeTree(node->rightChild, treeEncoding);
    }
}

HuffmanNode* decodeTree(const std::vector<unsigned char>& treeEncoding, size_t& position) {
    if (position >= treeEncoding.size()) return nullptr;
    if (treeEncoding[position] == 1) {
        position++;
        return new HuffmanNode(static_cast<char>(treeEncoding[position++]), 0);
    }
    position++;
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->leftChild = decodeTree(treeEncoding, position);
    node->rightChild = decodeTree(treeEncoding, position);
    return node;
}

std::string replaceReservedChars(const std::string& text) {
    std::string altered = text;
    const std::unordered_map<char, unsigned char> mappings = {
        {'\n', 0x80}, {'"', 0x81}, {'$', 0x82}, {'%', 0x83},
        {'&', 0x84}, {'(', 0x85}, {')', 0x86}, {'*', 0x87},
        {',', 0x88}, {'-', 0x89}, {'.', 0x8A}, {'\'', 0x8B}
    };
    for (char& c : altered) {
        if (mappings.count(c)) {
            c = mappings.at(c);
        }
    }
    return altered;
}

std::string restoreReservedChars(const std::string& text) {
    std::string restored = text;
    const std::unordered_map<unsigned char, char> mappings = {
        {0x80, '\n'}, {0x81, '"'}, {0x82, '$'}, {0x83, '%'},
        {0x84, '&'}, {0x85, '('}, {0x86, ')'}, {0x87, '*'},
        {0x88, ','}, {0x89, '-'}, {0x8A, '.'}, {0x8B, '\''}
    };
    for (char& c : restored) {
        if (mappings.count(static_cast<unsigned char>(c))) {
            c = mappings.at(static_cast<unsigned char>(c));
        }
    }
    return restored;
}

std::string compress(const std::string& input) {
    std::string transformed = replaceReservedChars(input);
    std::map<char, int> frequencyTable = calculateFrequencies(transformed);
    HuffmanNode* root = constructHuffmanTree(frequencyTable);
    std::unordered_map<char, std::string> huffmanMapping;
    createHuffmanCodes(root, "", huffmanMapping);
    std::vector<unsigned char> encodedTree;
    encodeTree(root, encodedTree);
    std::string compressedOutput;
    unsigned char tempByte = 0;
    int bitCounter = 0;
    for (char ch : transformed) {
        std::string code = huffmanMapping[ch];
        for (char bit : code) {
            tempByte <<= 1;
            if (bit == '1') tempByte |= 1;
            bitCounter++;
            if (bitCounter == 8) {
                compressedOutput.push_back(tempByte);
                tempByte = 0;
                bitCounter = 0;
            }
        }
    }
    if (bitCounter > 0) {
        tempByte <<= (8 - bitCounter);
        compressedOutput.push_back(tempByte);
    }
    std::string finalOutput(encodedTree.begin(), encodedTree.end());
    finalOutput += compressedOutput;
    return finalOutput;
}

std::string decompress(const std::string& compressedInput) {
    size_t pos = 0;
    std::vector<unsigned char> encodedTree(compressedInput.begin(), compressedInput.end());
    HuffmanNode* root = decodeTree(encodedTree, pos);
    std::string decompressedOutput;
    HuffmanNode* currentNode = root;
    while (pos < encodedTree.size()) {
        unsigned char byteVal = encodedTree[pos];
        for (int i = 7; i >= 0; --i) {
            bool bit = (byteVal >> i) & 1;
            currentNode = bit ? currentNode->rightChild : currentNode->leftChild;
            if (!currentNode->leftChild && !currentNode->rightChild) {
                decompressedOutput += currentNode->character;
                currentNode = root;
            }
        }
        pos++;
    }
    return restoreReservedChars(decompressedOutput);
}
