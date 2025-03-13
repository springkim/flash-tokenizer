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
#include<future>
#include <execution>
#include "robin_hood.h"
#include "env.h"
#include "debugging.h"
#include "version.h"

#ifdef _OPENMP
#include "omp.h"
#else
#include "thread_pool.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
#define SELECT_ANY  __declspec(selectany)
#elif defined(__linux__)
#define SELECT_ANY  __attribute__((weak))
#elif defined(__APPLE__)
#define SELECT_ANY  __attribute__((weak))
#endif

#define OPTIMIZED

#ifdef __clang__
#define PARALLEL_STL 0
#else
#define PARALLEL_STL 1
#endif

#define LIST 1
#define DEQUE 0
#define VECTOR 0

#if LIST == 1
using STRING_LIST = std::list<std::string>;
#define CONCAT(A, B) A.splice(A.end(),B)
#elif DEQUE == 1
using STRING_LIST = std::deque<std::string>;
#define CONCAT(A, B) std::move(B.begin(),B.end(),std::back_inserter(A))
#elif VECTOR == 1
using STRING_LIST = std::vector<std::string>;
#define CONCAT(A, B) std::move(B.begin(),B.end(),std::back_inserter(A))
#endif

#define LIST_IDS 0
#define DEQUE_IDS 0
#define VECTOR_IDS 1

using INTEGER_TYPE = int;
#if LIST_IDS == 1
using INT_LIST = std::list<INTEGER_TYPE>;
#define IDS_CONCAT(A, B) A.splice(A.end(),B)
#define INIT(A, B) ((void)0)
#define IDS_RETURN(A)   std::vector<int>((A).begin(),(A).end())
#elif DEQUE_IDS == 1
using INT_LIST = std::deque<INTEGER_TYPE>;
#define IDS_CONCAT(A, B) std::move(B.begin(),B.end(),std::back_inserter(A))
#define INIT(A,B) ((void)0)
#define IDS_RETURN(A)   std::vector<int>((A).begin(),(A).end())
#elif VECTOR_IDS == 1
using INT_LIST = std::vector<INTEGER_TYPE>;
#define IDS_CONCAT(A, B) std::move(B.begin(),B.end(),std::back_inserter(A))
#define INIT(A, B) (A).reserve((B))
#define IDS_RETURN(A)   (A)
#endif

static std::string reverse_string(std::string s) {
    std::reverse(s.begin(), s.end());
    return s;

    std::vector<std::vector<char> > b;
    int i = 0;
    while (i < s.length()) {
        std::vector<char> a;
        a.push_back(s[i]);
        if (s[i] < 0) {
            a.push_back(s[i + 1]);
            a.push_back(s[i + 2]);
            i += 2;
        }
        i++;
        b.push_back(a);
    }
    std::string rev;
    std::reverse(b.begin(), b.end());
    for (size_t i = 0; i < b.size(); i++) {
        for (size_t j = 0; j < b[i].size(); j++) {
            rev.push_back(b[i][j]);
        }
    }

    return rev;
}

OPTIMIZED class Vocab {
public:
    std::vector<std::string> tokens;
    robin_hood::unordered_flat_map<std::string, int> token_to_index;

    explicit Vocab(const std::string &filename, bool reverse = false) {
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
            if (reverse) {
                line = reverse_string(line);
            }
            tokens.push_back(line);
            token_to_index[line] = idx;
        }
        ifs.close();
    }

    [[nodiscard]] int get(const std::string &token, int default_value = 0) const {
        if (auto it = token_to_index.find(token); it != token_to_index.end())
            return it->second;
        return default_value;
    }
};


OPTIMIZED static std::vector<int>
convert_by_vocab(const Vocab &vocab, const STRING_LIST &items, int max_length = -1) {
    const size_t size = items.size();
    const size_t final_size = (max_length != -1 && static_cast<size_t>(max_length) > size)
                                  ? static_cast<size_t>(max_length)
                                  : size;
    std::vector<int> ids(final_size, 0);

    const auto &token_to_index = vocab.token_to_index;
    size_t index = 0;

    for (const auto &item: items) {
        auto it = token_to_index.find(item);
        ids[index] = (it != token_to_index.end() ? it->second : -1);
        index++;
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
                std::copy_n(p, skip, result.begin() + old_size);
                p += skip;
            } else {
                ++p;
            }
        }
    }

    return result;
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
    {0x4E00, 0x9FFF},
    {0x3400, 0x4DBF},
    {0xF900, 0xFAFF},
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
    using SplitFunction = STRING_LIST (*)(const std::string &);
    SplitFunction split_function = nullptr;

public:
    const bool do_lower_case;
    std::vector<char> char_map{};
    std::vector<bool> isSpace{};

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
        if (do_lower_case) {
            split_function = to_lower_split_on_punc;
        } else {
            split_function = split_on_punc;
        }
    }


    [[nodiscard]] STRING_LIST tokenize(const std::string &text) const {
        STRING_LIST output;
        std::string cleaned = clean_text(text, this->char_map);
        std::string tokenized = tokenize_chinese_chars(cleaned);
        STRING_LIST orig_tokens = whitespace_tokenize(tokenized, this->isSpace);
#if PARALLEL_STL == 1
        std::mutex mutex;
        std::for_each(std::execution::par, orig_tokens.begin(), orig_tokens.end(), [&](auto &token) {
            auto sub_tokens = this->split_function(token);
            std::lock_guard<std::mutex> lock(mutex);
            CONCAT(output, sub_tokens);
        });
#else
        for (const auto &token: orig_tokens) {
            auto sub_tokens = this->split_function(token);
            CONCAT(output, sub_tokens);
        }
#endif
        return output;
    }
};

class WordpieceTokenizer {
public:
    const Vocab &vocab;
    const std::string UNK;
    int UNK_NUM;

    explicit WordpieceTokenizer(const Vocab &vocab_, std::string unk = "[UNK]", int max_chars = 256)
        : vocab(vocab_), UNK(std::move(unk)) {
        this->UNK_NUM = vocab.get(this->UNK);
    }


    OPTIMIZED void tokenizer_ids(const std::string &token, int max_length, INT_LIST &input_ids) const {
        if (token.size() > static_cast<size_t>(max_length)) {
            input_ids.push_back(this->UNK_NUM);
            return;
        }
        int original_size = input_ids.size();
        size_t start = 0;
        std::string candidate;
        while (start < token.size()) {
            bool found = false;
            for (size_t len = token.size() - start; len > 0; --len) {
                candidate.clear();
                if (start)
                    candidate.append("##");
                candidate.append(token, start, len);
                if (auto val = vocab.get(candidate, -1); val != -1) {
                    found = true;
                    input_ids.emplace_back(val);
                    start += len;
                    break;
                }
            }
            if (!found) {
                input_ids.resize(original_size);
                input_ids.emplace_back(this->UNK_NUM);
                return;
            }
        }
    }

    OPTIMIZED [[nodiscard]] STRING_LIST tokenize(const std::string &token, int max_length) const {
        STRING_LIST output;
        if (token.size() > static_cast<size_t>(max_length)) {
            return {this->UNK};
        }
        size_t start = 0;
        std::string candidate;
        while (start < token.size()) {
            bool found = false;

            for (size_t len = token.size() - start; len > 0; --len) {
                candidate.clear();
                if (start)
                    candidate.append("##");
                candidate.append(token, start, len);
                auto it = vocab.token_to_index.find(candidate);
                if (it != vocab.token_to_index.end()) {
                    found = true;
                    output.emplace_back(candidate);
                    start += len;
                    break;
                }
            }
            if (!found) {
                return {this->UNK};
            }
        }
        return output;
    }
};

class WordpieceBackwardTokenizer {
public:
    const Vocab &vocab;
    const std::string UNK;
    int UNK_NUM = 0;
    int max_input_chars_per_word;

    explicit WordpieceBackwardTokenizer(const Vocab &vocab_, std::string unk = "[UNK]", int max_chars = 256)
        : vocab(vocab_), UNK(std::move(unk)), max_input_chars_per_word(max_chars) {
        this->UNK_NUM = vocab.get(this->UNK);
    }

    void tokenizer_ids(const std::string &token, int max_length, INT_LIST &input_ids) const {
        if (token.size() > static_cast<size_t>(max_length)) {
            input_ids.push_back(vocab.get(UNK));
        }
        int original_size = input_ids.size();
        size_t pos = token.size();
        std::string candidate;
        INT_LIST temp_ids;

        while (pos > 0) {
            bool found = false;
            size_t new_pos = pos;
            for (size_t i = 0; i < pos; ++i) {
                size_t len = pos - i;
                candidate.clear();
                if (i > 0)
                    candidate.append("##");
                candidate.append(token, i, len);
                if (auto id = vocab.get(candidate, -1); id != -1) {
                    found = true;
                    temp_ids.push_back(id);
                    new_pos = i;
                    break;
                }
            }
            if (!found) {
                input_ids.resize(original_size);
                input_ids.push_back(vocab.get(UNK));
                return;
            }
            pos = new_pos;
        }
        input_ids.insert(input_ids.end(), temp_ids.rbegin(), temp_ids.rend());
    }
};

class FlashBertTokenizer {
protected:
    const std::string UNK = "[UNK]";
    const std::string CLS = "[CLS]";
    const std::string SEP = "[SEP]";
    int CLS_NUM{};
    int SEP_NUM{};
#ifndef _OPENMP
    ThreadPool pool;
#endif

public:
    virtual ~FlashBertTokenizer() = default;

    const Vocab vocab;
    const BasicTokenizer basic;
    const WordpieceTokenizer wordpiece;
    std::string _version_ = "Unknown";

    explicit FlashBertTokenizer(const std::string &vocab_file, bool do_lower_case = true)
        : vocab(vocab_file), basic(do_lower_case),
          wordpiece(vocab, this->UNK) {
        this->CLS_NUM = vocab.get(this->CLS);
        this->SEP_NUM = vocab.get(this->SEP);
        this->_version_ = cpp_env(VERSION);
    }

    virtual std::string version() {
        return this->_version_;
    }

protected:
    STRING_LIST tokenize(const std::string &text, int max_length = 100) {
        const size_t allowed_length = max_length - 1;
        STRING_LIST output;
        output.emplace_back(this->CLS);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            auto wp_tokens = wordpiece.tokenize(token, max_length);
            CONCAT(output, wp_tokens);
            if (output.size() > allowed_length) {
                output.resize(allowed_length);
                break;
            }
        }
        output.emplace_back(this->SEP);
        return output;
    }


    virtual std::vector<int> tokenizer_ids(const std::string &text, int max_length, const std::string &padding) {
        const size_t allowed_length = max_length - 1;
        INT_LIST input_ids;
        INIT(input_ids, static_cast<int>(max_length * 1.2));

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            wordpiece.tokenizer_ids(token, max_length, input_ids);


            if (input_ids.size() > allowed_length) {
                input_ids.resize(allowed_length);
                break;
            }
        }
        input_ids.emplace_back(this->SEP_NUM);
        if (padding == "max_length") {
            input_ids.resize(max_length, 0);
        }
        return IDS_RETURN(input_ids);
    }

public:
    [[nodiscard]] std::vector<int>
    convert_tokens_to_ids(const STRING_LIST &tokens, int max_length = -1) const {
        return convert_by_vocab(vocab, tokens, max_length);
    }

    [[nodiscard]] std::vector<std::string> convert_ids_to_tokens(const std::vector<int> &ids) const {
        std::vector<std::string> tokens;
        tokens.reserve(ids.size());
        for (int id: ids) {
            tokens.push_back(
                (id >= 0 && id < static_cast<int>(this->vocab.tokens.size())) ? this->vocab.tokens[id] : this->UNK);
        }
        return tokens;
    }

    virtual std::vector<int> operator()(const std::string &text, const std::string &padding = "longest", int max_length = 100) {
        return this->tokenizer_ids(text, max_length, padding);
    }

#ifdef _OPENMP
    virtual std::vector<std::vector<int> > operator()(const std::vector<std::string> &texts,
                                                      const std::string &padding = "longest",
                                                      int max_length = 100) {
        std::vector<std::vector<int> > input_ids(texts.size());

#pragma omp parallel for
        for (int i = 0; i < static_cast<int>(texts.size()); i++) {
            input_ids[i] = tokenizer_ids(texts[i], max_length, padding);
        }
        return input_ids;
    }
#else
    virtual std::vector<std::vector<int>> operator()(const std::vector<std::string> &texts,
                                                     const std::string &padding = "longest",
                                                     int max_length = 100) {
        std::vector<std::future<std::invoke_result_t<decltype(&std::decay_t<decltype(*this)>::tokenizer_ids),
                decltype(this), const std::string&, int, const std::string&>>> futures;
        futures.reserve(texts.size());

        for (const auto &text : texts) {
            futures.push_back(pool.enqueue([this, &text, max_length, &padding] {
                return tokenizer_ids(text, max_length, padding);
            }));
        }

        std::vector<std::vector<int>> input_ids;
        input_ids.reserve(futures.size());
        for (auto &f : futures) {
            input_ids.push_back(f.get());
        }

        return input_ids;
    }
#endif
};

class FlashBertTokenizerBidirectional : public FlashBertTokenizer {
public:
    WordpieceBackwardTokenizer wordpiece_backward;


    explicit FlashBertTokenizerBidirectional(const std::string &vocab_file, bool do_lower_case = true) : FlashBertTokenizer(vocab_file, do_lower_case), wordpiece_backward(vocab, this->UNK) {
    }

protected:
    std::vector<int> tokenizer_ids(const std::string &text, int max_length, const std::string &padding) override {
        const size_t allowed_length = max_length - 1;
        INT_LIST input_ids;
        INIT(input_ids, static_cast<int>(max_length * 1.2));

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            INT_LIST i0, i1;
            wordpiece.tokenizer_ids(token, max_length, i0);
            wordpiece_backward.tokenizer_ids(token, max_length, i1);

            INT_LIST filtered_i0;
            std::copy_if(i0.begin(), i0.end(), std::back_inserter(filtered_i0),
                         [](int i) { return i > 4; });
            INT_LIST filtered_i1;
            std::copy_if(i1.begin(), i1.end(), std::back_inserter(filtered_i1),
                         [](int i) { return i > 4; });


            std::sort(filtered_i0.begin(), filtered_i0.end());
            std::sort(filtered_i1.begin(), filtered_i1.end());

            if (filtered_i0 < filtered_i1) {
                IDS_CONCAT(input_ids, i0);
            } else {
                IDS_CONCAT(input_ids, i1);
            }
            if (input_ids.size() > allowed_length) {
                input_ids.resize(allowed_length);
                break;
            }
        }
        input_ids.emplace_back(this->SEP_NUM);
        if (padding == "max_length") {
            input_ids.resize(max_length, 0);
        }
        return IDS_RETURN(input_ids);
    }

public:
    std::vector<int> operator()(const std::string &text, const std::string &padding = "longest", int max_length = -1) override {
        return this->tokenizer_ids(text, max_length, padding);
    }
};

#endif
