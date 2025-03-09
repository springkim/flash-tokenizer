//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                               FlashBertTokenizer
//
//               Copyright (C) 2025, Kim Bomm, all rights reserved.
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
#pragma once
#ifndef FLASHTOKENIZER_BERT_TOKENIZER_H
#define FLASHTOKENIZER_BERT_TOKENIZER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <fstream>

#define XXH_STATIC_LINKING_ONLY
#define XXH_INLINE_ALL

#include "xxhash.h"

struct XXHash {
    size_t operator()(const std::string &s) const {
        return XXH3_64bits(s.data(), s.size());
    }
};

class Vocab {
public:
    std::vector<std::string> tokens;
    std::unordered_map<std::string, int, XXHash> token_to_index;

    Vocab(const std::string &filename) {
        std::ifstream ifs(filename);
        if (!ifs) {
            std::cerr << "File not found" << filename << "\n";
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
};

std::vector<int> convert_by_vocab(const Vocab &vocab, const std::vector<std::string> &items, int max_length);

std::vector<std::string> whitespace_tokenize(const std::string &text);

std::string to_lower(const std::string &text);

std::string clean_text(const std::string &text);

std::vector<std::string> split_on_punc(const std::string &text);

class BasicTokenizer {
public:
    bool do_lower_case;

    BasicTokenizer(bool lower_case) : do_lower_case(lower_case) {}

    std::vector<std::string> tokenize(const std::string &text) {
        std::string cleaned = clean_text(text);
        std::string tokenized = tokenize_chinese_chars(cleaned);
        auto orig_tokens = whitespace_tokenize(tokenized);
        std::vector<std::string> output;
        for (const auto &token: orig_tokens) {
            std::string proc = do_lower_case ? to_lower(token) : token;
            auto subtokens = split_on_punc(proc);
            for (const auto &st: subtokens)
                if (!st.empty()) output.push_back(st);
        }
        return output;
    }

private:
    inline bool is_chinese_char(uint32_t cp) {
        return ((cp >= 0x4E00 && cp <= 0x9FFF) ||
                (cp >= 0x3400 && cp <= 0x4DBF) ||
                (cp >= 0x20000 && cp <= 0x2A6DF) ||
                (cp >= 0x2A700 && cp <= 0x2B73F) ||
                (cp >= 0x2B740 && cp <= 0x2B81F) ||
                (cp >= 0x2B820 && cp <= 0x2CEAF) ||
                (cp >= 0xF900 && cp <= 0xFAFF) ||
                (cp >= 0x2F800 && cp <= 0x2FA1F));
    }

    std::string tokenize_chinese_chars(const std::string &text) {
        std::string output;
        output.reserve(text.size() * 2);
        const char *p = text.data();
        const char *end = p + text.size();

        while (p < end) {
            uint32_t codepoint = 0;
            int bytes = 0;
            unsigned char c = static_cast<unsigned char>(*p);

            if (c < 0x80) {
                codepoint = c;
                bytes = 1;
            } else if ((c >> 5) == 0x6) {
                if (p + 1 >= end) break;
                codepoint = c & 0x1F;
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
                bytes = 2;
            } else if ((c >> 4) == 0xE) {
                if (p + 2 >= end) break;
                codepoint = c & 0x0F;
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[2]) & 0x3F);
                bytes = 3;
            } else if ((c >> 3) == 0x1E) {
                if (p + 3 >= end) break;
                codepoint = c & 0x07;
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[2]) & 0x3F);
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(p[3]) & 0x3F);
                bytes = 4;
            } else {
                ++p;
                continue;
            }

            if (is_chinese_char(codepoint)) {
                output.push_back(' ');
                output.append(p, bytes);
                output.push_back(' ');
            } else {
                output.append(p, bytes);
            }
            p += bytes;
        }
        return output;
    }

};

class WordpieceTokenizer {
public:
    const Vocab &vocab;
    std::string unk_token;
    int max_input_chars_per_word;

    WordpieceTokenizer(const Vocab &vocab_, const std::string &unk, int max_chars)
            : vocab(vocab_), unk_token(unk), max_input_chars_per_word(max_chars) {}

    std::vector<std::string> tokenize(const std::string &token) {
        std::vector<std::string> output;
        if (token.size() > static_cast<size_t>(max_input_chars_per_word)) {
            output.push_back(unk_token);
            return output;
        }
        size_t start = 0;
        while (start < token.size()) {
            size_t end = token.size();
            std::string cur_substr;
            bool found = false;
            while (start < end) {
                std::string substr = token.substr(start, end - start);
                if (start > 0)
                    substr.assign("##" + substr);

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
};

class FlashBertTokenizer {
    const char *UNKNOWN_TOKEN = "[UNK]";
public:
    Vocab vocab;
    BasicTokenizer basic;
    WordpieceTokenizer wordpiece;
    int max_ids_length = 0;

    explicit FlashBertTokenizer(const std::string &vocab_file, bool do_lower_case = true,
                                int max_input_chars_per_word = 256)
            : vocab(vocab_file), basic(do_lower_case),
              wordpiece(vocab, this->UNKNOWN_TOKEN, max_input_chars_per_word) {
        this->max_ids_length = max_input_chars_per_word;
    }

    std::vector<std::string> tokenize(const std::string &text, int max_length = -1) {
        if (max_length == -1) {
            max_length = this->max_ids_length;
        }
        std::vector<std::string> output;
        output.emplace_back("[CLS]");
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            auto wp_tokens = wordpiece.tokenize(token);
            output.insert(output.end(), wp_tokens.begin(), wp_tokens.end());
            if (output.size() > max_length - 1) {
                output.resize(max_length - 1);
                break;
            }
        }

        output.emplace_back("[SEP]");
        return output;
    }

    std::vector<int> convert_tokens_to_ids(const std::vector<std::string> &tokens, int max_length = -1) const {
        return convert_by_vocab(vocab, tokens, max_length);
    }

    std::vector<std::string> convert_ids_to_tokens(const std::vector<int> &ids) {
        std::vector<std::string> tokens;
        tokens.reserve(ids.size());
        for (int id: ids) {
            tokens.push_back(
                    (id >= 0 && id < static_cast<int>(vocab.tokens.size())) ? vocab.tokens[id] : this->UNKNOWN_TOKEN);
        }
        return tokens;
    }

    std::vector<int> operator()(const std::string &text, const std::string &padding = "longest", int max_length = -1,
                                const std::string &return_tensors = "np", bool truncation = true) {
        auto tokens = this->tokenize(text, max_length);
        auto ids = this->convert_tokens_to_ids(tokens, padding == "max_length" ? this->max_ids_length : -1);
        return ids;
    }
};

#endif