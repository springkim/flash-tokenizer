//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                                     Trie
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
#ifndef TRSZTUUWWNKHJCSXLLTZMWDCJWTELKQRTZELVMMUKFPSGUDNATGJJPDLUMQFPB
#define TRSZTUUWWNKHJCSXLLTZMWDCJWTELKQRTZELVMMUKFPSGUDNATGJJPDLUMQFPB

#include <vector>
#include <array>
#include <string>
#include <utility>

class Trie {
public:
    struct Node {
        int vocab_index;
        std::array<int, 256> children{};

        Node() : vocab_index(-1) {
            children.fill(-1);
        }
    };

protected:
    std::vector<Node> pool;

public:
    Trie() {
        pool.reserve(1024);
        pool.emplace_back();
    }

    void insert(const std::string &word, int index) {
        int current = 0;
        for (unsigned char c: word) {
            int child = this->pool[current].children[c];
            if (child == -1) {
                child = pool.size();
                this->pool[current].children[c] = child;
                this->pool.emplace_back();
            }
            current = child;
        }
        pool[current].vocab_index = index;
    }

    [[nodiscard]] std::pair<size_t, int> search(const std::string &token, size_t start) const {
        int current = 0;
        size_t best_length = 0;
        int best_index = -1;
        for (size_t pos = start; pos < token.size(); ++pos) {
            unsigned char c = token[pos];
            int child = this->pool[current].children[c];
            if (child == -1)
                break;
            current = child;
            if (pool[current].vocab_index != -1) {
                best_length = pos - start + 1;
                best_index = this->pool[current].vocab_index;
            }
        }
        return {best_length, best_index};
    }
};


#endif
