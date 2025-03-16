#ifndef FLASHTOKENIZER_BERT_TOKENIZER_CUDA_CUH
#define FLASHTOKENIZER_BERT_TOKENIZER_CUDA_CUH

#include <cuda_runtime.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

// Forward declarations
class CudaVocab;
class CudaBasicTokenizer;
class CudaWordpieceTokenizer;
class CudaFlashBertTokenizer;

// CUDA implementation of Vocab
class CudaVocab {
public:
    std::vector<std::string> tokens;
    std::unordered_map<std::string, int> token_to_index;
    
    CudaVocab(const std::string &filename);
};

// CUDA implementation of BasicTokenizer
class CudaBasicTokenizer {
public:
    bool do_lower_case;

    CudaBasicTokenizer(bool lower_case) : do_lower_case(lower_case) {}

    std::vector<std::string> tokenize(const std::string &text);
};

// CUDA implementation of WordpieceTokenizer
class CudaWordpieceTokenizer {
public:
    const CudaVocab &vocab;
    std::string unk_token;
    int max_input_chars_per_word;

    CudaWordpieceTokenizer(const CudaVocab &vocab_, const std::string &unk, int max_chars)
            : vocab(vocab_), unk_token(unk), max_input_chars_per_word(max_chars) {}

    std::vector<std::string> tokenize(const std::string &token);
};

// CUDA implementation of FlashBertTokenizer
class CudaFlashBertTokenizer {
    const char *UNKNOWN_TOKEN = "[UNK]";
public:
    CudaVocab vocab;
    CudaBasicTokenizer basic;
    CudaWordpieceTokenizer wordpiece;
    int max_length = 0;

    CudaFlashBertTokenizer(const std::string &vocab_file, bool do_lower_case = true, int max_input_chars_per_word = 256);
    
    std::vector<std::string> tokenize(const std::string &text);
    std::vector<int> convert_tokens_to_ids(const std::vector<std::string> &tokens);
    std::vector<std::string> convert_ids_to_tokens(const std::vector<int> &ids);
    std::vector<int> operator()(const std::string &text);
};

// Helper functions
std::string cuda_to_lower(const std::string &text);
std::vector<std::string> cuda_whitespace_tokenize(const std::string &text);
std::string cuda_clean_text(const std::string &text);
std::vector<std::string> cuda_split_on_punc(const std::string &text);
std::string cuda_tokenize_chinese_chars(const std::string &text);
std::vector<int> cuda_convert_by_vocab(const CudaVocab &vocab, const std::vector<std::string> &items);
bool is_chinese_char(char c);

#endif // FLASHTOKENIZER_BERT_TOKENIZER_CUDA_CUH
