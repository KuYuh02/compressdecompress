#include <iostream>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <sstream>

std::string compress(const std::string& source) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> compressed;
    std::string w;
    int dictSize = 256;
    for (char c : source) {
        std::string wc = w + c;
        if (dictionary.count(wc)) {
            w = wc;
        } else {
            compressed.push_back(dictionary.count(w) ? dictionary[w] : w[0]);
            dictionary[wc] = dictSize++;
            w = c;
        }
    }
    if (!w.empty())
        compressed.push_back(dictionary.count(w) ? dictionary[w] : w[0]);
    
    std::ostringstream oss;
    for (int code : compressed) {
        oss << code << " ";
    }
    return oss.str();
}

std::string decompress(const std::string& source) {
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[i] = std::string(1, i);
    }
    
    std::istringstream iss(source);
    std::vector<int> compressed;
    int value;
    while (iss >> value) {
        compressed.push_back(value);
    }
    
    std::string w(1, compressed[0]);
    std::string decompressed = w;
    int dictSize = 256;
    for (size_t i = 1; i < compressed.size(); i++) {
        std::string entry;
        if (dictionary.count(compressed[i])) {
            entry = dictionary[compressed[i]];
        } else if (compressed[i] == dictSize) {
            entry = w + w[0];
        }
        decompressed += entry;
        dictionary[dictSize++] = w + entry[0];
        w = entry;
    }
    return decompressed;
}
