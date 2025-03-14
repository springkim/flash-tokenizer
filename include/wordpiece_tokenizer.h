//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                               WordPieceTokenizer
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

#ifndef LGZU3ZG24UWVYE1LDJE878ETKXEZ7F0GC9FZDFY05STRLPM0MQPU9UA0Q52KESNW
#define LGZU3ZG24UWVYE1LDJE878ETKXEZ7F0GC9FZDFY05STRLPM0MQPU9UA0Q52KESNW

#include "defines.h"
#include "vocab.h"


#if USE_TRIE == 1
#include "trie.h"

class WordpieceTokenizer {
protected:
    const Vocab &vocab;
    const std::string UNK;
    int UNK_NUM;
    const std::string suffix_indicator = "##";
    Trie initialTrie;
    Trie suffixTrie;

public:
    virtual void buildTries() {
        for (const auto &entry: vocab.token_to_index) {
            const std::string &word = entry.first;
            const int &idx = entry.second;
            if (word.rfind(suffix_indicator, 0) == 0)
                suffixTrie.insert(word.substr(suffix_indicator.size()), idx);
            else
                initialTrie.insert(word, idx);
        }
    }

public:
    virtual ~WordpieceTokenizer() = default;

    explicit WordpieceTokenizer(const Vocab &vocab_, std::string unk = "[UNK]", int max_chars = 256)
        : vocab(vocab_), UNK(std::move(unk)) {
        UNK_NUM = vocab.get(UNK);
        WordpieceTokenizer::buildTries();
    }

    OPTIMIZED virtual void tokenizer_ids(const std::string &token, int max_length, INT_LIST &input_ids) const {
        if (token.size() > static_cast<size_t>(max_length)) {
            input_ids.push_back(UNK_NUM);
            return;
        }
        const size_t original_size = input_ids.size();
        size_t start = 0;
        while (start < token.size()) {
            const Trie &trie = (start == 0 ? initialTrie : suffixTrie);
            auto [match_length, match_idx] = trie.search(token, start);
            if (match_idx == -1) {
                input_ids.resize(original_size);
                input_ids.push_back(UNK_NUM);
                return;
            }
            input_ids.push_back(match_idx);
            start += match_length;
        }
    }
};

#else


class WordpieceTokenizer {
protected:
    const Vocab &vocab;
    const std::string UNK;
    int UNK_NUM;
    const std::string suffix_indicator = "##";

public:
    virtual ~WordpieceTokenizer() = default;

    explicit WordpieceTokenizer(const Vocab &vocab_, std::string unk = "[UNK]", int max_chars = 256)
        : vocab(vocab_), UNK(std::move(unk)) {
        this->UNK_NUM = vocab.get(this->UNK);
    }


    OPTIMIZED virtual void tokenizer_ids(const std::string &token, int max_length, INT_LIST &input_ids) const {
        if (token.size() > static_cast<size_t>(max_length)) {
            input_ids.push_back(this->UNK_NUM);
            return;
        }
        const int original_size = input_ids.size();
        size_t start = 0;
        std::string candidate;
        while (start < token.size()) {
            bool found = false;
            for (size_t len = token.size() - start; len > 0; --len) {
                candidate.clear();
                if (start) {
                    candidate.append(this->suffix_indicator);
                }
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

    template<typename StringList>
    OPTIMIZED [[nodiscard]] StringList tokenize(const std::string &token, int max_length) const {
        StringList output;
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
                    candidate.append(this->suffix_indicator);
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

#endif


#endif
