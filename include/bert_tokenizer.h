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
#include "charmap.h"

class FlashBertTokenizer {
protected:
    const std::string UNK = "[UNK]";
    const std::string CLS = "[CLS]";
    const std::string SEP = "[SEP]";
    int CLS_NUM{};
    int SEP_NUM{};
    int UNK_NUM{};
    const int model_max_length;
    std::unique_ptr<ThreadPool> pool{};


    const Vocab vocab;
    const BasicTokenizer basic;
    const WordpieceTokenizer wordpiece;
    std::string _version_ = "Unknown";

public:
    explicit FlashBertTokenizer(const std::string &vocab_file, const bool do_lower_case = true,
                                const int model_max_length = -1)
        : vocab(vocab_file), basic(do_lower_case), model_max_length(model_max_length),
          wordpiece(vocab, this->UNK) {
        this->CLS_NUM = vocab.get(this->CLS);
        this->SEP_NUM = vocab.get(this->SEP);
        this->UNK_NUM = vocab.get(this->UNK);
        this->_version_ = cpp_env(VERSION_INFO);
        if (accent_mapping.empty()) {
            accent_mapping = initializeCharMap();
        }
    }

    virtual ~FlashBertTokenizer() = default;

    virtual std::string version() {
        return this->_version_;
    }


    virtual std::vector<int> tokenizer_ids(const std::string &text, const int max_length, const std::string &padding) {
        const size_t allowed_length = max_length - 1;
        std::vector<int> input_ids;
        input_ids.reserve(1024);

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            if (wordpiece.tokenizer_ids(token, max_length - 1, input_ids) == allowed_length) {
                break;
            }
        }
        input_ids.emplace_back(this->SEP_NUM);
        if (padding == "max_length") {
            input_ids.resize(max_length, 0);
        }

        return input_ids;
    }


    virtual std::vector<int> encode(const std::string &text, const std::string &padding = "max_length", int max_length = -1) {
        if (max_length == -1) {
            max_length = this->model_max_length;
        }
        return this->tokenizer_ids(text, max_length, padding);
    }

    virtual std::vector<std::vector<int> > batch_encode(const std::vector<std::string> &texts, const std::string &padding = "max_length", int max_length = -1, const bool parallel = true) {
        if (max_length == -1) {
            max_length = this->model_max_length;
        }
        if (parallel) {
#ifndef _OPENMP
            if (!this->pool) {
                this->pool = std::make_unique<ThreadPool>();
            }
            std::vector<std::future<std::invoke_result_t<decltype(&std::decay_t<decltype(*this)>::tokenizer_ids
                ),
                decltype(this), const std::string &, int, const std::string &> > > futures;
            futures.reserve(texts.size());

            for (const auto &text: texts) {
                futures.push_back(pool->enqueue([this, &text, max_length, &padding] {
                    return this->tokenizer_ids(text, max_length, padding);
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
                input_ids[i] = this->tokenizer_ids(texts[i], max_length, padding);
            }

            return input_ids;
#endif
        } else {
            std::vector<std::vector<int> > input_ids;
            input_ids.reserve(texts.size());
            for (auto &text: texts) {
                input_ids.push_back(this->encode(text, padding, max_length));
            }

            return input_ids;
        }
    }
};


class FlashBertTokenizerBidirectional : public FlashBertTokenizer {
protected:
    WordpieceBackwardTokenizer wordpiece_backward;
    std::unique_ptr<ThreadPool> pool2{};

public:
    explicit
    FlashBertTokenizerBidirectional(const std::string &vocab_file, const bool do_lower_case = true, const int model_max_length = -1) : FlashBertTokenizer(
                                                                                                                                           vocab_file, do_lower_case, model_max_length), wordpiece_backward(vocab, this->UNK) {
        this->pool2 = std::make_unique<ThreadPool>(2);
    }


    std::vector<int> tokenizer_ids(const std::string &text, const int max_length, const std::string &padding) override {
        const size_t allowed_length = max_length - 1;
        std::vector<int> input_ids;
        input_ids.reserve(1024);

        std::vector<int> i0, i1;
        std::vector<int> filtered_i0, filtered_i1;

        i0.reserve(max_length);
        i1.reserve(max_length);
        filtered_i0.reserve(max_length);
        filtered_i1.reserve(max_length);

        input_ids.emplace_back(this->CLS_NUM);
        auto basic_tokens = basic.tokenize(text);
        for (const auto &token: basic_tokens) {
            i0.clear();
            i1.clear();
            filtered_i0.clear();
            filtered_i1.clear();
            wordpiece.tokenizer_ids(token, max_length - 1, i0);
            wordpiece_backward.tokenizer_ids(token, max_length - 1, i1);
            if (i0 == i1) {
                std::move(i0.begin(), i0.end(), std::back_inserter(input_ids));
            } else {
                std::copy_if(i0.begin(), i0.end(), std::back_inserter(filtered_i0), [](int i) { return i > 4; });
                std::copy_if(i1.begin(), i1.end(), std::back_inserter(filtered_i1), [](int i) { return i > 4; });
                std::vector<int> &target = compare_ids(filtered_i0, filtered_i1) ? i0 : i1;
                std::move(target.begin(), target.end(), std::back_inserter(input_ids));
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
        return input_ids;
    }

    std::vector<int> encode(const std::string &text, const std::string &padding = "max_length", int max_length = -1) override {
        if (max_length == -1) {
            max_length = this->model_max_length;
        }
        return this->tokenizer_ids(text, max_length, padding);
    }

    std::vector<std::vector<int> > batch_encode(const std::vector<std::string> &texts, const std::string &padding = "max_length", int max_length = -1, const bool parallel = true) override {
        if (max_length == -1) {
            max_length = this->model_max_length;
        }
        if (parallel) {
#ifndef _OPENMP
            if (!this->pool) {
                this->pool = std::make_unique<ThreadPool>();
            }
            std::vector<std::future<std::invoke_result_t<decltype(&std::decay_t<decltype(*this)>::tokenizer_ids
                ),
                decltype(this), const std::string &, int, const std::string &> > > futures;
            futures.reserve(texts.size());

            for (const auto &text: texts) {
                futures.push_back(pool->enqueue([this, &text, max_length, &padding] {
                    return this->tokenizer_ids(text, max_length, padding);
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
                input_ids[i] = this->tokenizer_ids(texts[i], max_length, padding);
            }

            return input_ids;
#endif
        } else {
            std::vector<std::vector<int> > input_ids;
            input_ids.reserve(texts.size());
            for (auto &text: texts) {
                input_ids.push_back(this->encode(text, padding, max_length));
            }

            return input_ids;
        }
    }
};

#endif
