#ifndef FLASHTOKENIZER_CUDA_TOKENIZER_H
#define FLASHTOKENIZER_CUDA_TOKENIZER_H

#include <vector>
#include <string>
#include <unordered_map>

// Forward declaration of CUDA functions
extern "C" void launch_tokenize_kernel(const char* input, int* output, int input_length, int* output_length);

// CUDA-accelerated tokenizer class
class CudaTokenizer {
private:
    std::unordered_map<std::string, int> vocab;
    
public:
    CudaTokenizer(const std::string& vocab_file);
    
    // Process text using CUDA acceleration
    std::vector<int> tokenize(const std::string& text);
};

#endif // FLASHTOKENIZER_CUDA_TOKENIZER_H
