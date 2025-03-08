//
// Created by SPRING on 2025. 3. 8..
//
#include "bert_tokenizer.h"

std::vector<int> convert_by_vocab(const Vocab &vocab, const std::vector <std::string> &items) {
    std::vector<int> ids;
    ids.reserve(items.size());
    for (const auto &item: items) {
        auto it = vocab.token_to_index.find(item);
        ids.push_back(it != vocab.token_to_index.end() ? it->second : -1);
    }
    return ids;
}

std::vector <std::string> whitespace_tokenize(const std::string &text) {
    std::vector <std::string> tokens;
    size_t i = 0, n = text.size();
    while (i < n) {
        while (i < n && std::isspace(text[i])) i++;
        if (i >= n) break;
        size_t j = i;
        while (j < n && !std::isspace(text[j])) j++;
        tokens.push_back(text.substr(i, j - i));
        i = j;
    }
    return tokens;
}

std::string to_lower(const std::string &text) {
    std::string res = text;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

std::string clean_text(const std::string &text) {
    std::string res;
    res.reserve(text.size());
    for (unsigned char c: text) {
        if (c == 0)
            continue;
        res.push_back(std::isspace(c) ? ' ' : c);
    }
    return res;
}

std::vector <std::string> split_on_punc(const std::string &text) {
    std::vector <std::string> tokens;
    std::string buffer;
    for (char c: text) {
        if (std::ispunct(c)) {
            if (!buffer.empty()) {
                tokens.emplace_back(buffer);
                buffer.clear();
            }
            tokens.push_back(std::string(1, c));
        } else if (std::isspace(c)) {
            if (!buffer.empty()) {
                tokens.emplace_back(buffer);
                buffer.clear();
            }
        } else {
            buffer.push_back(c);
        }
    }
    if (!buffer.empty())
        tokens.emplace_back(buffer);
    return tokens;
}