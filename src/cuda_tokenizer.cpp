#include "cuda_tokenizer.h"
#include <fstream>
#include <iostream>
#include <algorithm>

CudaTokenizer::CudaTokenizer(const std::string& vocab_file) {
    // Load vocabulary from file
    std::ifstream ifs(vocab_file);
    if (!ifs) {
        std::cerr << "Failed to open vocabulary file: " << vocab_file << std::endl;
        exit(1);
    }
    
    std::string line;
    int idx = 0;
    while (std::getline(ifs, line)) {
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (line.empty()) continue;
        vocab[line] = idx++;
    }
}

std::vector<int> CudaTokenizer::tokenize(const std::string& text) {
    if (text.empty()) return {};
    
    // Prepare input for CUDA kernel
    const char* input = text.c_str();
    int input_length = text.length();
    
    // Allocate output buffer (worst case: every character is a token)
    std::vector<int> output_indices(input_length);
    int output_length = 0;
    
    // Launch CUDA kernel
    launch_tokenize_kernel(input, output_indices.data(), input_length, &output_length);
    
    // Resize output to actual length
    output_indices.resize(output_length);
    
    // Convert character indices to actual tokens and then to token IDs
    // This is a simplified version - in a real implementation, we would do proper tokenization
    std::vector<int> token_ids;
    for (int idx : output_indices) {
        std::string token(1, text[idx]);
        auto it = vocab.find(token);
        if (it != vocab.end()) {
            token_ids.push_back(it->second);
        } else {
            // Unknown token
            token_ids.push_back(-1);
        }
    }
    
    return token_ids;
}
