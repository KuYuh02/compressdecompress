#include <iostream>
#include <zlib.h>
#include <stdexcept>
#include <sstream>
#include <cstring>

std::string compress(const std::string& input, int level = Z_BEST_COMPRESSION) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    if (deflateInit(&stream, level) != Z_OK) {
        throw std::runtime_error("Failed to initialize compression.");
    }

    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
    stream.avail_in = input.size();
    
    char buffer[10240];
    std::string compressedData;
    int status;
    
    do {
        stream.next_out = reinterpret_cast<Bytef*>(buffer);
        stream.avail_out = sizeof(buffer);
        status = deflate(&stream, Z_FINISH);
        if (compressedData.size() < stream.total_out) {
            compressedData.append(buffer, stream.total_out - compressedData.size());
        }
    } while (status == Z_OK);
    
    deflateEnd(&stream);
    if (status != Z_STREAM_END) {
        throw std::runtime_error("Error occurred during compression.");
    }
    
    return compressedData;
}

std::string decompress(const std::string& input) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    if (inflateInit(&stream) != Z_OK) {
        throw std::runtime_error("Failed to initialize decompression.");
    }
    
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
    stream.avail_in = input.size();
    
    char buffer[10240];
    std::string decompressedData;
    int status;
    
    do {
        stream.next_out = reinterpret_cast<Bytef*>(buffer);
        stream.avail_out = sizeof(buffer);
        status = inflate(&stream, 0);
        if (decompressedData.size() < stream.total_out) {
            decompressedData.append(buffer, stream.total_out - decompressedData.size());
        }
    } while (status == Z_OK);
    
    inflateEnd(&stream);
    if (status != Z_STREAM_END) {
        throw std::runtime_error("Error occurred during decompression.");
    }
    
    return decompressedData;
}
