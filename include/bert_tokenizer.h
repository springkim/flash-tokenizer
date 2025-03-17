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
#ifndef MPNVTDXZKUBGNRMLPKGYLBWAARLLHQUSXVSQJDZTBGHQEJXGPMTNHHDFCVLMMU
#define MPNVTDXZKUBGNRMLPKGYLBWAARLLHQUSXVSQJDZTBGHQEJXGPMTNHHDFCVLMMU

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <map>
#include <list>
#include <functional>
#include <thread>
#include<future>

#include "robin_hood.h"
#include "env.h"
#include "debugging.h"
#include "thread_pool.h"
#include "version.h"
#include "defines.h"
#include "functions.h"
#include "vocab.h"

#include "basic_tokenizer.h"
#include "wordpiece_tokenizer.h"
#include "wordpiecebackward_tokenizer.h"


class FlashBertTokenizer {
protected:
    const std::string UNK = "[UNK]";
    const std::string CLS = "[CLS]";
    const std::string SEP = "[SEP]";
    int CLS_NUM{};
    int SEP_NUM{};
    int UNK_NUM{};

    std::unique_ptr<ThreadPool> pool{};


    const Vocab vocab;
    const BasicTokenizer basic;
    const WordpieceTokenizer wordpiece;
    std::string _version_ = "Unknown";

public:
    explicit FlashBertTokenizer(const std::string &vocab_file, bool do_lower_case = true)
        : vocab(vocab_file), basic(do_lower_case),
          wordpiece(vocab, this->UNK) {
        this->CLS_NUM = vocab.get(this->CLS);
        this->SEP_NUM = vocab.get(this->SEP);
        this->UNK_NUM = vocab.get(this->UNK);
        this->_version_ = cpp_env(VERSION);
    }

    virtual ~FlashBertTokenizer() = default;

    virtual std::string version() {
        return this->_version_;
    }


    // template<typename StringList>
    // StringList tokenize(const std::string &text, int max_length = 100) {
    //     const size_t allowed_length = max_length - 1;
    //     StringList output;
    //     output.emplace_back(this->CLS);
    //     auto basic_tokens = basic.tokenize<StringList>(text);
    //     for (const auto &token: basic_tokens) {
    //         auto wp_tokens = wordpiece.tokenize(token, max_length);
    //         CONCAT(output, wp_tokens);
    //         if (output.size() > allowed_length) {
    //             output.resize(allowed_length);
    //             break;
    //         }
    //     }
    //     output.emplace_back(this->SEP);
    //     return output;
    // }

    template<typename StringList=STRING_LIST>
    std::vector<int> tokenizer_ids(const std::string &text, int max_length, const std::string &padding) {
        const size_t allowed_length = max_length - 1;
        INT_LIST input_ids;
        INIT(input_ids, static_cast<int>(max_length * 1.2));

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize<STRING_LIST>(text);
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

    [[nodiscard]] std::vector<int>

    convert_tokens_to_ids(const std::vector<std::string> &tokens, int max_length = -1) const {
        return convert_by_vocab(vocab, tokens, max_length);
    }

    [[nodiscard]] std::vector<std::string> convert_ids_to_tokens(const std::vector<int> &ids) const {
        std::vector<std::string> tokens;
        tokens.reserve(ids.size());
        for (const int id: ids) {
            tokens.push_back(
                (id >= 0 && id < static_cast<int>(this->vocab.tokens.size())) ? this->vocab.tokens[id] : this->UNK);
        }
        return tokens;
    }

    virtual std::vector<int> encode(const std::string &text, const std::string &padding, int max_length) {
        return this->tokenizer_ids<STRING_LIST>(text, max_length, padding);
    }

    virtual std::vector<std::vector<int> > batch_encode(const std::vector<std::string> &texts, const std::string &padding, int max_length) {
#ifndef _OPENMP
        if (!this->pool) {
            this->pool = std::make_unique<ThreadPool>();
        }
        std::vector<std::future<std::invoke_result_t<decltype(&std::decay_t<decltype(*this)>::tokenizer_ids<STRING_LIST>
            ),
            decltype(this), const std::string &, int, const std::string &> > > futures;
        futures.reserve(texts.size());

        for (const auto &text: texts) {
            futures.push_back(pool->enqueue([this, &text, max_length, &padding] {
                return this->tokenizer_ids<STRING_LIST>(text, max_length, padding);
            }));
        }

        std::vector<std::vector<int> > input_ids;
        input_ids.reserve(futures.size());
        for (auto &f: futures) {
            input_ids.push_back(f.get());
        }

        return input_ids;
#else
        std::vector<std::vector<int>> input_ids(texts.size());

#pragma omp parallel for
        for (size_t i = 0; i < texts.size(); ++i) {
            input_ids[i] = this->tokenizer_ids<STRING_LIST>(texts[i], max_length, padding);
        }

        return input_ids;
#endif
    }
};

class FlashBertTokenizerBidirectional : public FlashBertTokenizer {
protected:
    WordpieceBackwardTokenizer wordpiece_backward;

public:
    explicit
    FlashBertTokenizerBidirectional(const std::string &vocab_file, bool do_lower_case = true) : FlashBertTokenizer(
                                                                                                    vocab_file, do_lower_case), wordpiece_backward(vocab, this->UNK) {
    }

    template<typename StringList>
    std::vector<int> tokenizer_ids(const std::string &text, int max_length, const std::string &padding) {
        const size_t allowed_length = max_length - 1;
        INT_LIST input_ids;
        INIT(input_ids, static_cast<int>(max_length * 1.2));

        INT_LIST i0, i1;
        INT_LIST filtered_i0, filtered_i1;
        i0.reserve(max_length);
        i1.reserve(max_length);
        filtered_i0.reserve(max_length);
        filtered_i1.reserve(max_length);

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize<StringList>(text);
        for (const auto &token: basic_tokens) {
            i0.clear();
            i1.clear();
            filtered_i0.clear();
            filtered_i1.clear();

            wordpiece.tokenizer_ids(token, max_length, i0);
            wordpiece_backward.tokenizer_ids(token, max_length, i1);

            std::copy_if(i0.begin(), i0.end(), std::back_inserter(filtered_i0), [](int i) { return i > 4; });
            std::copy_if(i1.begin(), i1.end(), std::back_inserter(filtered_i1), [](int i) { return i > 4; });

            std::sort(filtered_i0.begin(), filtered_i0.end());
            std::sort(filtered_i1.begin(), filtered_i1.end());

            IDS_CONCAT(input_ids, filtered_i0 < filtered_i1 ? i0 : i1);
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

    std::vector<int> encode(const std::string &text, const std::string &padding, int max_length) override {
        return this->tokenizer_ids<STRING_LIST_FAST>(text, max_length, padding);
    }

    std::vector<std::vector<int> > batch_encode(const std::vector<std::string> &texts, const std::string &padding, int max_length) override {
#ifndef _OPENMP
        if (!this->pool) {
            this->pool = std::make_unique<ThreadPool>();
        }
        std::vector<std::future<std::invoke_result_t<decltype(&std::decay_t<decltype(*this)>::tokenizer_ids<STRING_LIST>
            ),
            decltype(this), const std::string &, int, const std::string &> > > futures;
        futures.reserve(texts.size());

        for (const auto &text: texts) {
            futures.push_back(pool->enqueue([this, &text, max_length, &padding] {
                return this->tokenizer_ids<STRING_LIST>(text, max_length, padding);
            }));
        }

        std::vector<std::vector<int> > input_ids;
        input_ids.reserve(futures.size());
        for (auto &f: futures) {
            input_ids.push_back(f.get());
        }

        return input_ids;
#else
        std::vector<std::vector<int>> input_ids(texts.size());

#pragma omp parallel for
        for (size_t i = 0; i < texts.size(); ++i) {
            input_ids[i] = this->tokenizer_ids<STRING_LIST>(texts[i], max_length, padding);
        }

        return input_ids;
#endif
    }
};

#endif
