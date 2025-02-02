#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <iostream>
#include <bitset>

class HuffmanNode {
public:
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char character, int frequency) 
        : ch(character), freq(frequency), left(nullptr), right(nullptr) {}

    // Comparator for priority queue (min-heap)
    struct Compare {
        bool operator()(HuffmanNode* a, HuffmanNode* b) {
            return a->freq > b->freq;
        }
    };
};
std::string compress(const std::string& source) {
    // Step 1: Build frequency table
    std::unordered_map<char, int> freqTable;
    for (char ch : source) {
        freqTable[ch]++;
    }

    // Step 2: Build a priority queue for the Huffman tree
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNode::Compare> pq;
    for (const auto& entry : freqTable) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }

    // Step 3: Build the Huffman tree
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        pq.push(merged);
    }

    HuffmanNode* root = pq.top();

    // Step 4: Generate Huffman codes from the tree
    std::unordered_map<char, std::string> huffmanCodes;
    std::function<void(HuffmanNode*, const std::string&)> generateCodes = 
        [&](HuffmanNode* node, const std::string& code) {
            if (!node) return;
            if (node->ch != '\0') {  // Leaf node, contains a character
                huffmanCodes[node->ch] = code;
            }
            generateCodes(node->left, code + "0");
            generateCodes(node->right, code + "1");
        };
    
    generateCodes(root, "");

    // Step 5: Create the compressed binary string
    std::string compressedData;
    for (char ch : source) {
        compressedData += huffmanCodes[ch];
    }

    // Step 6: Store the tree structure and codes for decompression
    std::string header;
    for (const auto& entry : huffmanCodes) {
        header += entry.first + ":" + std::to_string(entry.second.size()) + ";";
    }

    return header + "\n" + compressedData;
}
std::string decompress(const std::string& source) {
    // Step 1: Extract header and compressed data
    size_t pos = source.find("\n");
    std::string header = source.substr(0, pos);
    std::string compressedData = source.substr(pos + 1);

    // Step 2: Rebuild the Huffman tree from the header
    std::unordered_map<std::string, char> huffmanCodeReverse;
    size_t start = 0;
    while (start < header.size()) {
        size_t end = header.find(";", start);
        std::string entry = header.substr(start, end - start);
        size_t colonPos = entry.find(":");
        char ch = entry[0];
        int codeLength = std::stoi(entry.substr(colonPos + 1));
        std::string code = compressedData.substr(0, codeLength);
        huffmanCodeReverse[code] = ch;
        compressedData = compressedData.substr(codeLength);  // Remove used code
        start = end + 1;
    }

    // Step 3: Decode the compressed data
    std::string decoded;
    std::string currentCode;
    for (char bit : compressedData) {
        currentCode += bit;
        if (huffmanCodeReverse.find(currentCode) != huffmanCodeReverse.end()) {
            decoded += huffmanCodeReverse[currentCode];
            currentCode.clear();
        }
    }

    return decoded;
}

