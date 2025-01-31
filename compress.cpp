#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <sstream>

using namespace std;

// Huffman Tree Node
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}

    // Compare function for priority queue
    bool operator>(const Node& other) const {
        return freq > other.freq;
    }
};

// Compare function for priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Encode map for compression
unordered_map<char, string> huffmanCodes;
unordered_map<string, char> reverseHuffmanCodes;

// Build the Huffman Tree and generate codes
Node* buildHuffmanTree(const string& source) {
    unordered_map<char, int> freq;
    for (char c : source) freq[c]++;

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& p : freq)
        pq.push(new Node(p.first, p.second));

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    return pq.top();
}

// Generate Huffman Codes
void generateCodes(Node* root, string code) {
    if (!root) return;
    if (root->ch != '\0') {
        huffmanCodes[root->ch] = code;
        reverseHuffmanCodes[code] = root->ch;
    }
    generateCodes(root->left, code + "0");
    generateCodes(root->right, code + "1");
}

// Convert a binary string to its ASCII representation
string binaryToString(const string& binary) {
    stringstream ss;
    for (size_t i = 0; i < binary.length(); i += 8) {
        bitset<8> bits(binary.substr(i, 8));
        ss << static_cast<char>(bits.to_ulong());
    }
    return ss.str();
}

// Convert an ASCII string to its binary representation
string stringToBinary(const string& str) {
    string binary;
    for (char c : str) {
        binary += bitset<8>(c).to_string();
    }
    return binary;
}

// Compress function
string compress(const string& source) {
    if (source.empty()) return "";

    // Build Huffman Tree and generate codes
    Node* root = buildHuffmanTree(source);
    huffmanCodes.clear();
    reverseHuffmanCodes.clear();
    generateCodes(root, "");

    // Encode input string
    string encodedString;
    for (char c : source) {
        encodedString += huffmanCodes[c];
    }

    // Store the Huffman codes as a header
    string header;
    for (auto& p : huffmanCodes) {
        header += p.first + ":" + p.second + "|";
    }

    return header + "#" + binaryToString(encodedString);
}

string decompress(const string& source) {
    if (source.empty()) return "";

    // Locate the header delimiter #
    size_t delimiter = source.find('#');
    if (delimiter == string::npos) return "";

    string header = source.substr(0, delimiter);
    string encodedData = source.substr(delimiter + 1);
    string binaryData = stringToBinary(encodedData);

    // Step 1: Rebuild the Huffman Table from the header
    unordered_map<string, char> huffmanTable;
    size_t pos = 0;
    
    while (pos < header.size()) {
        size_t colon = header.find(':', pos);
        if (colon == string::npos) break;

        size_t bar = header.find('|', colon);
        if (bar == string::npos) break;

        char c = header[pos]; // The character
        string code = header.substr(colon + 1, bar - colon - 1); // The Huffman code

        huffmanTable[code] = c;
        pos = bar + 1;
    }
    // Step 2: Decode the binary stream using the Huffman Table
    string decodedString;
    string currentCode;

    for (char bit : binaryData) {
        currentCode += bit;
        if (huffmanTable.count(currentCode)) {
            decodedString += huffmanTable[currentCode];
            currentCode.clear();
        }
    }

    return decodedString;
}
