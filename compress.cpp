#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bitset>

constexpr int DICT_SIZE = 256;

// Function to compress a string using a simple dictionary-based approach
std::string compress(const std::string& source) {
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < DICT_SIZE; ++i) {
        dictionary[std::string(1, i)] = i;
    }
    
    std::string current;
    std::vector<int> encoded;
    int nextCode = DICT_SIZE;
    
    for (char ch : source) {
        std::string temp = current + ch;
        if (dictionary.find(temp) != dictionary.end()) {
            current = temp;
        } else {
            encoded.push_back(dictionary[current]);
            dictionary[temp] = nextCode++;
            current = std::string(1, ch);
        }
    }
    
    if (!current.empty()) {
        encoded.push_back(dictionary[current]);
    }
    
    std::string compressed;
    for (int code : encoded) {
        compressed += std::bitset<16>(code).to_string();
    }
    return compressed;
}

// Function to decompress a string
std::string decompress(const std::string& source) {
    std::vector<int> encoded;
    for (size_t i = 0; i < source.size(); i += 16) {
        encoded.push_back(std::bitset<16>(source.substr(i, 16)).to_ulong());
    }
    
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < DICT_SIZE; ++i) {
        dictionary[i] = std::string(1, i);
    }
    
    std::string previous = dictionary[encoded[0]];
    std::string decompressed = previous;
    std::string entry;
    int nextCode = DICT_SIZE;
    
    for (size_t i = 1; i < encoded.size(); ++i) {
        int currentCode = encoded[i];
        if (dictionary.find(currentCode) != dictionary.end()) {
            entry = dictionary[currentCode];
        } else {
            entry = previous + previous[0];
        }
        decompressed += entry;
        dictionary[nextCode++] = previous + entry[0];
        previous = entry;
    }
    
    return decompressed;
}
