#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <sstream>

// Huffman Node Structure
class HuffmanNode {
public:
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char character, int frequency)
        : ch(character), freq(frequency), left(nullptr), right(nullptr) {}

    // Comparator for the priority queue (min-heap)
    struct Compare {
        bool operator()(HuffmanNode* a, HuffmanNode* b) {
            return a->freq > b->freq;
        }
    };
};

// Function to build the frequency table for the input string
std::unordered_map<char, int> buildFrequencyTable(const std::string& source) {
    std::unordered_map<char, int> frequencyTable;
    for (char ch : source) {
        frequencyTable[ch]++;
    }
    return frequencyTable;
}

// Function to build the Huffman tree from the frequency table
HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& frequencyTable) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNode::Compare> pq;

    // Create a node for each character and insert into the priority queue
    for (const auto& entry : frequencyTable) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }

    // Build the tree by combining two least frequent nodes
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq);  // Internal node
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();  // Root of the Huffman Tree
}

// Function to generate the Huffman codes by traversing the tree
void generateHuffmanCodes(HuffmanNode* node, const std::string& code,
                           std::unordered_map<char, std::string>& huffmanCodes) {
    if (!node) return;

    // If it's a leaf node, store the code
    if (node->ch != '\0') {
        huffmanCodes[node->ch] = code;
    }

    // Recur for left and right subtrees
    generateHuffmanCodes(node->left, code + "0", huffmanCodes);
    generateHuffmanCodes(node->right, code + "1", huffmanCodes);
}

// Function to compress the string using Huffman Encoding
std::string compress(const std::string& source) {
    // Step 1: Build the frequency table
    std::unordered_map<char, int> frequencyTable = buildFrequencyTable(source);

    // Step 2: Build the Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencyTable);

    // Step 3: Generate the Huffman codes
    std::unordered_map<char, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Step 4: Encode the source string
    std::string compressedData;
    for (char ch : source) {
        compressedData += huffmanCodes[ch];
    }

    // Step 5: Prepare the header with Huffman codes and their characters
    std::stringstream headerStream;
    for (const auto& entry : huffmanCodes) {
        headerStream << entry.first << ":" << entry.second << " ";
    }

    // Step 6: Return the header + compressed data as the final output
    return headerStream.str() + "\n" + compressedData;
}

// Function to decompress the string using Huffman Encoding
std::string decompress(const std::string& source) {
    // Step 1: Parse the header and extract the Huffman codes
    size_t newlinePos = source.find("\n");
    std::string header = source.substr(0, newlinePos);
    std::string compressedData = source.substr(newlinePos + 1);

    std::unordered_map<std::string, char> reverseHuffmanCodes;

    std::stringstream headerStream(header);
    std::string codePair;
    while (headerStream >> codePair) {
        size_t colonPos = codePair.find(":");
        char ch = codePair[0];
        std::string code = codePair.substr(colonPos + 1);

        reverseHuffmanCodes[code] = ch;
    }

    // Step 2: Decode the compressed data
    std::string decodedString;
    std::string currentCode;
    for (char bit : compressedData) {
        currentCode += bit;
        if (reverseHuffmanCodes.find(currentCode) != reverseHuffmanCodes.end()) {
            decodedString += reverseHuffmanCodes[currentCode];
            currentCode.clear();
        }
    }

    return decodedString;
}
