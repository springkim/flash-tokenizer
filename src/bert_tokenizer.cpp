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
#include "bert_tokenizer.h"

std::vector<int> convert_by_vocab(const Vocab &vocab, const std::vector<std::string> &items, int max_length = -1) {
    std::vector<int> ids;
    ids.reserve(items.size());
    for (const auto &item: items) {
        auto it = vocab.token_to_index.find(item);
        ids.push_back(it != vocab.token_to_index.end() ? it->second : -1);
    }
    if (max_length != -1) {
        while (ids.size() < max_length) {
            ids.push_back(0);
        }
    }
    return ids;
}

std::vector<std::string> whitespace_tokenize(const std::string &text) {
    std::vector<std::string> tokens;
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
    std::string res;
    res.reserve(text.size());

    const char* p = text.data();
    const char* end = p + text.size();

    while (p < end) {
        unsigned char c = static_cast<unsigned char>(*p);

        if (c < 0x80) {

            res.push_back(std::tolower(c));
            p++;
        } else if ((c >> 5) == 0x6) {

            if (p + 1 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
        } else if ((c >> 4) == 0xE) {

            if (p + 2 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
        } else if ((c >> 3) == 0x1E) {

            if (p + 3 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
        } else {

            p++;
        }
    }

    return res;
}

std::string clean_text(const std::string &text) {
    std::string res;
    res.reserve(text.size());

    const char* p = text.data();
    const char* end = p + text.size();

    while (p < end) {
        unsigned char c = static_cast<unsigned char>(*p);

        if (c == 0) {
            p++;
            continue;
        }

        if (c < 0x80) {

            res.push_back(std::isspace(c) ? ' ' : c);
            p++;
        } else if ((c >> 5) == 0x6) {

            if (p + 1 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
        } else if ((c >> 4) == 0xE) {

            if (p + 2 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
        } else if ((c >> 3) == 0x1E) {

            if (p + 3 >= end) break;
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
            res.push_back(*p++);
        } else {

            p++;
        }
    }

    return res;
}

std::vector<std::string> split_on_punc(const std::string &text) {
    std::vector<std::string> tokens;
    std::string buffer;

    const char* p = text.data();
    const char* end = p + text.size();

    while (p < end) {
        unsigned char c = static_cast<unsigned char>(*p);

        if (c < 0x80) {

            if (std::ispunct(c)) {

                if (!buffer.empty()) {
                    tokens.emplace_back(buffer);
                    buffer.clear();
                }
                tokens.push_back(std::string(1, c));
                p++;
            } else if (std::isspace(c)) {

                if (!buffer.empty()) {
                    tokens.emplace_back(buffer);
                    buffer.clear();
                }
                p++;
            } else {

                buffer.push_back(c);
                p++;
            }
        } else {

            if ((c >> 5) == 0x6) {

                if (p + 1 >= end) break;
                buffer.push_back(*p++);
                buffer.push_back(*p++);
            } else if ((c >> 4) == 0xE) {

                if (p + 2 >= end) break;
                buffer.push_back(*p++);
                buffer.push_back(*p++);
                buffer.push_back(*p++);
            } else if ((c >> 3) == 0x1E) {

                if (p + 3 >= end) break;
                buffer.push_back(*p++);
                buffer.push_back(*p++);
                buffer.push_back(*p++);
                buffer.push_back(*p++);
            } else {

                p++;
            }
        }
    }

    if (!buffer.empty())
        tokens.emplace_back(buffer);

    return tokens;
}