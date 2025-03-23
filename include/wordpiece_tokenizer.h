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
            if (word.rfind(suffix_indicator, 0) == 0) {
                suffixTrie.insert(word.substr(suffix_indicator.size()), idx);
            } else {
                initialTrie.insert(word, idx);
            }
        }
        initialTrie.build();
        suffixTrie.build();
    }

    virtual ~WordpieceTokenizer() = default;

    explicit WordpieceTokenizer(const Vocab &vocab_, std::string unk = "[UNK]")
        : vocab(vocab_), UNK(std::move(unk)) {
        this->UNK_NUM = vocab.get(UNK);
        WordpieceTokenizer::buildTries();
    }

    virtual int tokenizer_ids(const std::string &token, const int max_length, std::vector<int> &input_ids) const {
        if (token.size() > static_cast<size_t>(max_length) && input_ids.size() < max_length) {
            input_ids.emplace_back(this->UNK_NUM);
            return input_ids.size();
        }
        const size_t original_size = input_ids.size();
        size_t start = 0;
        while (start < token.size()) {
            const auto &trie = (start != 0 ? this->suffixTrie : this->initialTrie);
            auto [match_length, match_idx] = trie.search(token, start);
            if (match_idx == -1) {
                input_ids.resize(original_size);
                if (input_ids.size() < max_length)
                    input_ids.emplace_back(this->UNK_NUM);
                return input_ids.size();
            }
            if (input_ids.size() < max_length) {
                input_ids.emplace_back(match_idx);
            } else
                return input_ids.size();
            start += match_length;
        }
        return input_ids.size();
    }
};

#endif
