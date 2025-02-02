#include <string>
#include <unordered_map>

using namespace std;

// Function to create a frequency table for characters in the input string
unordered_map<char, int> build_frequency_table(const string& source) {
    unordered_map<char, int> freq_table;
    for (char c : source) {
        freq_table[c]++;
    }
    return freq_table;
}

// Function to compress a string using a frequency table
string compress(const string& source) {
    unordered_map<char, int> freq_table = build_frequency_table(source);
    
    string compressed;
    for (char c : source) {
        compressed += c + to_string(freq_table[c]);
    }
    return compressed;
}

// Function to decompress a compressed string
string decompress(const string& compressed) {
    string decompressed;
    for (int i = 0; i < compressed.size(); i++) {
        char ch = compressed[i];
        int count = 0;
        while (isdigit(compressed[++i])) {
            count = count * 10 + (compressed[i] - '0');
        }
        i--; // Adjust index to point back to the character
        for (int j = 0; j < count; j++) {
            decompressed += ch;
        }
    }
    return decompressed;
}
