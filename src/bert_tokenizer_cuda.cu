#include "bert_tokenizer_cuda.cuh"
#include <iostream>
#include <fstream>
#include <algorithm>

// CUDA Vocab implementation
CudaVocab::CudaVocab(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Failed to open file: " << filename << "\n";
        std::exit(1);
    }
    std::string line;
    while (std::getline(ifs, line)) {
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (line.empty()) continue;
        int idx = tokens.size();
        tokens.push_back(line);
        token_to_index[line] = idx;
    }
}

// Helper function to check if a character is Chinese
bool is_chinese_char(char c) {
    // This is a simplified version - in a real implementation,
    // we would need to handle UTF-8 encoding properly
    // For now, we'll return false as a placeholder
    return false;
}

// Implementation of to_lower
std::string cuda_to_lower(const std::string &text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
        [](char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }
    );
    
    return result;
}

// CUDA implementation of whitespace_tokenize
std::vector<std::string> cuda_whitespace_tokenize(const std::string &text) {
    int n = text.size();
    if (n == 0) return {};
    
    // Process on host for now to avoid CUDA compatibility issues
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (int i = 0; i < n; i++) {
        char c = text[i];
        bool is_whitespace = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        
        if (is_whitespace) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

// CUDA implementation of clean_text
std::string cuda_clean_text(const std::string &text) {
    int n = text.size();
    if (n == 0) return "";
    
    // Process on host for now to avoid CUDA compatibility issues
    std::string result;
    result.reserve(n);
    
    for (int i = 0; i < n; i++) {
        unsigned char c = text[i];
        if (c == 0) {
            // Skip null characters
            continue;
        }
        
        // Replace whitespace with space
        bool is_space = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        char new_char = is_space ? ' ' : c;
        
        result += new_char;
    }
    
    return result;
}

// CUDA implementation of split_on_punc
std::vector<std::string> cuda_split_on_punc(const std::string &text) {
    int n = text.size();
    if (n == 0) return {};
    
    // Process on host for now to avoid CUDA compatibility issues
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (int i = 0; i < n; i++) {
        char c = text[i];
        bool is_punct = (c == '.' || c == ',' || c == '!' || c == '?' || c == ';' || 
                        c == ':' || c == '"' || c == '\'' || c == '(' || c == ')' || 
                        c == '[' || c == ']' || c == '{' || c == '}' || c == '-' || 
                        c == '_' || c == '+' || c == '=' || c == '<' || c == '>' || 
                        c == '/' || c == '\\' || c == '@' || c == '#' || c == '$' || 
                        c == '%' || c == '^' || c == '&' || c == '*' || c == '~' || c == '`');
        
        if (is_punct) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            tokens.push_back(text.substr(i, 1));
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

// CUDA implementation of tokenize_chinese_chars
std::string cuda_tokenize_chinese_chars(const std::string &text) {
    int n = text.size();
    if (n == 0) return "";
    
    // Process on host for now to avoid CUDA compatibility issues
    std::string result;
    result.reserve(n * 3); // Reserve space for worst case (all Chinese chars)
    
    for (int i = 0; i < n; i++) {
        if (is_chinese_char(text[i])) {
            result += " ";
            result += text[i];
            result += " ";
        } else {
            result += text[i];
        }
    }
    
    return result;
}

// CUDA implementation of convert_by_vocab
std::vector<int> cuda_convert_by_vocab(const CudaVocab &vocab, const std::vector<std::string> &items) {
    std::vector<int> ids(items.size());
    
    // This is a simplified version - in a real implementation,
    // we would use a more efficient algorithm for vocabulary lookup
    
    for (size_t i = 0; i < items.size(); i++) {
        auto it = vocab.token_to_index.find(items[i]);
        ids[i] = it != vocab.token_to_index.end() ? it->second : -1;
    }
    
    return ids;
}

// CudaBasicTokenizer::tokenize implementation
std::vector<std::string> CudaBasicTokenizer::tokenize(const std::string &text) {
    std::string cleaned = cuda_clean_text(text);
    std::string tokenized = cuda_tokenize_chinese_chars(cleaned);
    auto orig_tokens = cuda_whitespace_tokenize(tokenized);
    std::vector<std::string> output;
    
    for (const auto &token : orig_tokens) {
        std::string proc = do_lower_case ? cuda_to_lower(token) : token;
        auto subtokens = cuda_split_on_punc(proc);
        for (const auto &st : subtokens)
            if (!st.empty()) output.push_back(st);
    }
    
    return output;
}

// CudaWordpieceTokenizer::tokenize implementation
std::vector<std::string> CudaWordpieceTokenizer::tokenize(const std::string &token) {
    std::vector<std::string> output;
    
    if (token.size() > static_cast<size_t>(max_input_chars_per_word)) {
        output.push_back(unk_token);
        return output;
    }
    
    // This is a simplified version - in a real implementation,
    // we would use a more efficient algorithm for wordpiece tokenization
    
    size_t start = 0;
    while (start < token.size()) {
        size_t end = token.size();
        std::string cur_substr;
        bool found = false;
        
        while (start < end) {
            std::string substr = token.substr(start, end - start);
            if (start > 0)
                substr = "##" + substr;
            
            if (vocab.token_to_index.count(substr)) {
                cur_substr = substr;
                found = true;
                break;
            }
            end--;
        }
        
        if (!found) {
            output.push_back(unk_token);
            return output;
        }
        
        output.push_back(cur_substr);
        start = end;
    }
    
    return output;
}

// CudaFlashBertTokenizer implementation
CudaFlashBertTokenizer::CudaFlashBertTokenizer(const std::string &vocab_file, bool do_lower_case, int max_input_chars_per_word)
    : vocab(vocab_file), basic(do_lower_case), wordpiece(vocab, UNKNOWN_TOKEN, max_input_chars_per_word) {
    this->max_length = max_input_chars_per_word;
}

std::vector<std::string> CudaFlashBertTokenizer::tokenize(const std::string &text) {
    std::vector<std::string> output;
    output.emplace_back("[CLS]");
    
    auto basic_tokens = basic.tokenize(text);
    for (const auto &token : basic_tokens) {
        auto wp_tokens = wordpiece.tokenize(token);
        output.insert(output.end(), wp_tokens.begin(), wp_tokens.end());
        if (output.size() > this->max_length - 1) {
            output.resize(this->max_length - 1);
            break;
        }
    }
    
    output.emplace_back("[SEP]");
    return output;
}

std::vector<int> CudaFlashBertTokenizer::convert_tokens_to_ids(const std::vector<std::string> &tokens) {
    return cuda_convert_by_vocab(vocab, tokens);
}

std::vector<std::string> CudaFlashBertTokenizer::convert_ids_to_tokens(const std::vector<int> &ids) {
    std::vector<std::string> tokens;
    tokens.reserve(ids.size());
    for (int id : ids) {
        tokens.push_back(
            (id >= 0 && id < static_cast<int>(vocab.tokens.size())) ? vocab.tokens[id] : this->UNKNOWN_TOKEN);
    }
    return tokens;
}

std::vector<int> CudaFlashBertTokenizer::operator()(const std::string &text) {
    auto tokens = this->tokenize(text);
    auto ids = this->convert_tokens_to_ids(tokens);
    return ids;
}
