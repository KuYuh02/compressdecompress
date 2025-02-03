#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>

class HuffmanNode {
public:
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

class CompareNodes {
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

std::unordered_map<char, std::string> generateHuffmanCodes(HuffmanNode* root) {
    std::unordered_map<char, std::string> huffCodes;
    std::function<void(HuffmanNode*, std::string)> traverse = [&](HuffmanNode* node, std::string code) {
        if (!node) return;
        if (node->ch != '\0') {
            huffCodes[node->ch] = code;
        }
        traverse(node->left, code + "0");
        traverse(node->right, code + "1");
    };
    traverse(root, "");
    return huffCodes;
}

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
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

std::string compress(const std::string& source) {
    std::unordered_map<char, int> freqMap;
    for (char c : source) {
        freqMap[c]++;
    }
    HuffmanNode* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffCodes = generateHuffmanCodes(root);
    
    std::ostringstream freqStream;
    freqStream << freqMap.size() << " ";
    for (const auto& pair : freqMap) {
        freqStream << pair.first << " " << pair.second << " ";
    }
    
    std::string compressedData;
    for (char c : source) {
        compressedData += huffCodes[c];
    }
    
    std::ostringstream bitStream;
    int padding = 8 - (compressedData.size() % 8);
    for (int i = 0; i < padding; ++i) {
        compressedData += "0";
    }
    bitStream << padding << " ";
    
    for (size_t i = 0; i < compressedData.size(); i += 8) {
        std::bitset<8> bits(compressedData.substr(i, 8));
        bitStream << static_cast<char>(bits.to_ulong());
    }
    
    return freqStream.str() + "|" + bitStream.str();
}

std::string decompress(const std::string& source) {
    size_t delim = source.find('|');
    std::istringstream freqStream(source.substr(0, delim));
    int numEntries;
    freqStream >> numEntries;
    
    std::unordered_map<char, int> freqMap;
    for (int i = 0; i < numEntries; i++) {
        char ch;
        int freq;
        freqStream >> ch >> freq;
        freqMap[ch] = freq;
    }
    
    HuffmanNode* root = buildHuffmanTree(freqMap);
    
    std::istringstream bitStream(source.substr(delim + 1));
    int padding;
    bitStream >> padding;
    
    std::string bitString;
    char ch;
    while (bitStream.get(ch)) {
        bitString += std::bitset<8>(static_cast<unsigned char>(ch)).to_string();
    }
    
    if (padding > 0) {
        bitString = bitString.substr(0, bitString.size() - padding);
    }
    
    std::string decoded;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    
    return decoded;
}
