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
   
}
std::string decompress(const std::string& source) {
    
}

