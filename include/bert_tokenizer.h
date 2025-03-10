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
#include <utility>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <map>
#include <forward_list>
#include <list>
#include <functional>
#include <thread>
#include "robin_hood.h"

#if defined(_WIN32) || defined(_WIN64)
#define SELECT_ANY  __declspec(selectany)
#elif defined(__linux__)
#define SELECT_ANY  __attribute__((weak))
#elif defined(__APPLE__)
#define SELECT_ANY  __attribute__((weak))
#endif

#define OPTIMIZED

#define LIST 1
#define DEQUE 0

#if LIST == 1
using STRING_LIST = std::list<std::string>;
#define CONCAT(A, B) A.splice(A.end(),B)
#elif DEQUE == 1
using STRING_LIST = std::deque<std::string>;
#define CONCAT(A, B) std::move(B.begin(),B.end(),std::back_inserter(A))
#endif


OPTIMIZED class Vocab {
public:
    std::vector<std::string> tokens;
    robin_hood::unordered_flat_map<std::string, int> token_to_index;


    explicit Vocab(const std::string &filename) {
        std::ifstream ifs(filename);
        if (!ifs) {
            std::cerr << "File not found" << filename << "\n";
            std::exit(1);
        }
        std::string line;
        while (std::getline(ifs, line)) {
            line.erase(line.find_last_not_of(" \n\r\t") + 1);
            if (line.empty()) continue;
            auto idx = static_cast<int>(tokens.size());
            tokens.push_back(line);
            token_to_index[line] = idx;
        }
        ifs.close();
    }
};


OPTIMIZED static std::vector<int>
convert_by_vocab(const Vocab &vocab, const STRING_LIST &items, int max_length = -1) {
    const size_t size = items.size();
    const size_t final_size = (max_length != -1 && static_cast<size_t>(max_length) > size)
                              ? static_cast<size_t>(max_length) : size;
    std::vector<int> ids(final_size, 0);

    const auto &token_to_index = vocab.token_to_index;
    size_t index = 0;

    for (const auto &item: items) {
        auto it = token_to_index.find(item);
        ids[index++] = (it != token_to_index.end() ? it->second : -1);
    }
    return ids;
}


OPTIMIZED static STRING_LIST whitespace_tokenize(const std::string &text, const std::vector<bool> &isSpace) {

    STRING_LIST tokens;
    const char *data = text.data();
    const char *end = data + text.size();
    while (data < end) {
        while (data < end && isSpace[static_cast<unsigned char>(*data)])
            ++data;

        if (data >= end)
            break;

        const char *start = data;
        while (data < end && !isSpace[static_cast<unsigned char>(*data)])
            ++data;
        tokens.emplace_back(start, data - start);
    }
    return tokens;
}


static std::string clean_text(const std::string &text, const std::vector<char> &char_map) {
    if (text.empty()) return {};


    std::string result;
    result.reserve(text.size());

    const char *p = text.data();
    const char *end = p + text.size();

    while (p < end) {
        auto c = static_cast<unsigned char>(*p);

        if (c == 0) {
            ++p;
            continue;
        }

        if (c < 0x80) {
            result.push_back(char_map[c]);
            ++p;
        } else {
            int skip = 0;

            if ((c & 0xE0) == 0xC0 && p + 1 < end) {
                skip = 2;
            } else if ((c & 0xF0) == 0xE0 && p + 2 < end) {
                skip = 3;
            } else if ((c & 0xF8) == 0xF0 && p + 3 < end) {
                skip = 4;
            }

            if (skip > 0) {
                size_t old_size = result.size();
                result.resize(old_size + skip);
                memmove(&result[old_size], p, skip);
                p += skip;
            } else {
                ++p;
            }
        }
    }

    return result;
}


inline char to_lower_ascii(char c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

static std::string to_lower(const std::string &text) {
    size_t n = text.size();
    std::string res(n, '\0');
    const char *p = text.data();
    const char *end = p + n;
    char *out = &res[0];
    while (p < end) {
        auto c = static_cast<unsigned char>(*p);
        if (c < 0x80) {
            *out++ = to_lower_ascii(*p++);
        } else {
            int seq_len = (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : (c >> 3) == 0x1E ? 4 : 1;
            if (p + seq_len > end) break;
            if (seq_len >= 1 && seq_len <= 4) {
                std::copy(p, p + seq_len, out);
            } else {
                out[0] = p[0];
            }
            p += seq_len;
            out += seq_len;
        }
    }
    return res;
}

static STRING_LIST split_on_punc(const std::string &text) {
    STRING_LIST tokens;
    std::string buffer;
    const char *p = text.data();
    const char *end = p + text.size();

    auto flush = [&]() {
        if (!buffer.empty()) {
            tokens.push_back(std::move(buffer));
            buffer.clear();
        }
    };

    while (p < end) {
        auto c = static_cast<unsigned char>(*p);
        if (c < 0x80) {
            if (std::ispunct(c)) {
                flush();
                tokens.emplace_back(1, c);
                ++p;
            } else if (std::isspace(c)) {
                flush();
                ++p;
            } else {
                buffer.push_back(c);
                ++p;
            }
        } else {
            int len = 1;
            if ((c & 0xE0) == 0xC0)
                len = 2;
            else if ((c & 0xF0) == 0xE0)
                len = 3;
            else if ((c & 0xF8) == 0xF0)
                len = 4;
            if (p + len > end)
                break;
            buffer.append(p, len);
            p += len;
        }
    }
    flush();
    return tokens;
}


OPTIMIZED static STRING_LIST to_lower_split_on_punc(const std::string &text) {
    STRING_LIST tokens;
    std::string buffer;
    const char *p = text.data(), *end = p + text.size();
    while (p < end) {
        auto c = static_cast<unsigned char>(*p);
        if (c < 0x80) {
            char lc = (c >= 'A' && c <= 'Z') ? c + 32 : c;
            if (std::ispunct(static_cast<unsigned char>(lc))) {
                if (!buffer.empty()) {
                    tokens.push_back(std::move(buffer));
                    buffer.clear();
                }
                tokens.emplace_back(1, lc);
                ++p;
            } else if (std::isspace(static_cast<unsigned char>(lc))) {
                if (!buffer.empty()) {
                    tokens.push_back(std::move(buffer));
                    buffer.clear();
                }
                ++p;
            } else {
                buffer.push_back(lc);
                ++p;
            }
        } else {
            int len = (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 1;
            if (p + len > end) break;
            buffer.append(p, len);
            p += len;
        }
    }
    if (!buffer.empty())
        tokens.push_back(std::move(buffer));
    return tokens;
}

class ChineseCharDetector {
private:
    struct Range {
        uint32_t start, end;

        bool operator<(const Range &other) const {
            return end < other.start;
        }
    };

    static const std::vector<Range> ranges;

public:
    static bool isChineseChar(uint32_t codepoint) {
        if (codepoint < 0x3400 || codepoint > 0x2FA1F)
            return false;

        return std::any_of(ranges.begin(), ranges.end(), [codepoint](const auto &range) {
            return codepoint >= range.start && codepoint <= range.end;
        });
    }
};

SELECT_ANY const std::vector<ChineseCharDetector::Range> ChineseCharDetector::ranges = {
        {0x4E00,  0x9FFF},
        {0x3400,  0x4DBF},
        {0xF900,  0xFAFF},
        {0x20000, 0x2A6DF},
        {0x2A700, 0x2B73F},
        {0x2B740, 0x2B81F},
        {0x2B820, 0x2CEAF},
        {0x2F800, 0x2FA1F}
};

OPTIMIZED static std::string tokenize_chinese_chars(const std::string &text) {
    std::string output;
    output.reserve(text.size() * 2);
    const char *p = text.data(), *end = p + text.size();

    while (p < end) {
        uint32_t codepoint = 0;
        int bytes = 0;
        auto c = static_cast<unsigned char>(*p);

        if (c < 0x80) {
            codepoint = c;
            bytes = 1;
        } else {
            int len = (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : (c < 0xF8) ? 4 : 0;
            if (!len || p + len > end)
                break;

            if (len == 2) {
                codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
            } else if (len == 3) {
                codepoint = ((c & 0x0F) << 12) |
                            ((static_cast<unsigned char>(p[1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(p[2]) & 0x3F);
            } else if (len == 4) {
                codepoint = ((c & 0x07) << 18) |
                            ((static_cast<unsigned char>(p[1]) & 0x3F) << 12) |
                            ((static_cast<unsigned char>(p[2]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(p[3]) & 0x3F);
            }
            bytes = len;
        }
        if (ChineseCharDetector::isChineseChar(codepoint)) {
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

class BasicTokenizer {
public:
    const bool do_lower_case;
    std::vector<char> char_map;
    std::vector<bool> isSpace;

    explicit BasicTokenizer(bool lower_case) : do_lower_case(lower_case) {
        char_map.assign(256, 0);
        for (int i = 0; i < 256; ++i) {
            if (i == 0) {
                char_map[i] = 0;
            } else if (i < 128) {
                char_map[i] = std::isspace(i) ? ' ' : i;
            } else {
                char_map[i] = i;
            }
        }
        isSpace.assign(256, false);
        isSpace[' '] = true;
        isSpace['\t'] = true;
        isSpace['\n'] = true;
        isSpace['\v'] = true;
        isSpace['\f'] = true;
        isSpace['\r'] = true;
    }


    [[nodiscard]] STRING_LIST tokenize(const std::string &text) const {
        STRING_LIST output;
        std::string cleaned = clean_text(text, this->char_map);
        std::string tokenized = tokenize_chinese_chars(cleaned);
        STRING_LIST orig_tokens = whitespace_tokenize(tokenized, this->isSpace);

        if (!this->do_lower_case) {
            for (const auto &token: orig_tokens) {
                auto sub_tokens = to_lower_split_on_punc(token);
                CONCAT(output, sub_tokens);
            }
        } else {
            for (const auto &token: orig_tokens) {
                auto sub_tokens = split_on_punc(to_lower(token));
                CONCAT(output, sub_tokens);
            }
        }
        return output;
    }
};

class WordpieceTokenizer {
public:
    const Vocab &vocab;
    const std::string UNK;
    int max_input_chars_per_word;

    explicit WordpieceTokenizer(const Vocab &vocab_, std::string unk = "[UNK]", int max_chars = 256)
            : vocab(vocab_), UNK(std::move(unk)), max_input_chars_per_word(max_chars) {
    }

    OPTIMIZED [[nodiscard]] STRING_LIST tokenize(const std::string &token) const {
        STRING_LIST output;
        if (token.size() > static_cast<size_t>(max_input_chars_per_word))
            return {this->UNK};

        size_t start = 0;
        std::string candidate;
        while (start < token.size()) {
            bool found = false;

            for (size_t len = token.size() - start; len > 0; --len) {
                candidate.clear();
                if (start)
                    candidate.append("##");
                candidate.append(token, start, len);
                if (vocab.token_to_index.find(candidate) != vocab.token_to_index.end()) {
                    found = true;
                    output.emplace_back(candidate);
                    start += len;
                    break;
                }
            }
            if (!found)
                return {this->UNK};
        }
        return output;
    }

};

class FlashBertTokenizer {
    const std::string UNK = "[UNK]";
    const std::string CLS = "[CLS]";
    const std::string SEP = "[SEP]";
public:
    Vocab vocab;
    BasicTokenizer basic;
    WordpieceTokenizer wordpiece;
    int max_ids_length = 0;

    explicit FlashBertTokenizer(const std::string &vocab_file, bool do_lower_case = true,
                                int max_input_chars_per_word = 256)
            : vocab(vocab_file), basic(do_lower_case),
              wordpiece(vocab, this->UNK, max_input_chars_per_word) {
        this->max_ids_length = max_input_chars_per_word;

    }

    STRING_LIST tokenize(const std::string &text, int max_length = -1) {
        const int allowed_length = max_length == -1 ? this->max_ids_length - 1 : max_length - 1;
        STRING_LIST output;
        output.emplace_back(this->CLS);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            auto wp_tokens = wordpiece.tokenize(token);
            CONCAT(output, wp_tokens);
            if (output.size() > allowed_length) {
                output.resize(allowed_length);
                break;
            }
        }
        output.emplace_back(this->SEP);
        return output;
    }


    [[nodiscard]] std::vector<int>
    convert_tokens_to_ids(const STRING_LIST &tokens, int max_length = -1) const {
        return convert_by_vocab(vocab, tokens, max_length);
    }

    std::vector<std::string> convert_ids_to_tokens(const std::vector<int> &ids) {
        std::vector<std::string> tokens;
        tokens.reserve(ids.size());
        for (int id: ids) {
            tokens.push_back(
                    (id >= 0 && id < static_cast<int>(vocab.tokens.size())) ? vocab.tokens[id] : this->UNK);
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