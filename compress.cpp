#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>
#include <cstdint>

class HuffmanNode {
public:
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    ~HuffmanNode() {
        delete left;
        delete right;
    }
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
    freqStream.put(static_cast<char>(freqMap.size()));
    for (const auto& pair : freqMap) {
        freqStream.put(pair.first);
        freqStream.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
    
    std::vector<uint8_t> compressedData;
    std::string bitString;
    for (char c : source) {
        bitString += huffCodes[c];
    }
    int padding = (8 - (bitString.size() % 8)) % 8;
    bitString.append(padding, '0');
    compressedData.push_back(static_cast<uint8_t>(padding));
    
    for (size_t i = 0; i < bitString.size(); i += 8) {
        std::bitset<8> bits(bitString.substr(i, 8));
        compressedData.push_back(static_cast<uint8_t>(bits.to_ulong()));
    }
    
    std::ostringstream result;
    result.write(freqStream.str().c_str(), freqStream.str().size());
    result.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
    
    delete root;
    return result.str();
}

std::string decompress(const std::string& source) {
    std::istringstream freqStream(source);
    int numEntries = freqStream.get();
    
    std::unordered_map<char, int> freqMap;
    for (int i = 0; i < numEntries; i++) {
        char ch = freqStream.get();
        int freq;
        freqStream.read(reinterpret_cast<char*>(&freq), sizeof(int));
        freqMap[ch] = freq;
    }
    
    HuffmanNode* root = buildHuffmanTree(freqMap);
    int padding = freqStream.get();
    
    std::string bitString;
    char ch;
    while (freqStream.get(ch)) {
        bitString += std::bitset<8>(static_cast<unsigned char>(ch)).to_string();
    }
    
    bitString = bitString.substr(0, bitString.size() - padding);
    
    std::string decoded;
    HuffmanNode* current = root;
    for (char bit : bitString) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    
    delete root;
    return decoded;
}
