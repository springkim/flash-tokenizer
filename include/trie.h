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
#include <queue>


struct TrieNode {
    TRIE_INTEGER vocab_index{};
    std::array<TRIE_INTEGER, 256> children{};

    explicit TrieNode() : vocab_index(-1) {
        children.fill(-1);
    }
};

struct AhocorasickTrieNode : public TrieNode {
    std::array<bool, 256> explicitChild{};
    TRIE_INTEGER fail;

    AhocorasickTrieNode(): TrieNode(), fail(0) {
        children.fill(-1);
        explicitChild.fill(false);
    }
};

#if USE_AHOCORASICK == 1
class Trie {
public:
    std::vector<AhocorasickTrieNode> pool{};

    Trie() {
        pool.reserve(1024);
        pool.emplace_back();
    }

    FORCE_INLINE void insert(const std::string &word, int index) {
        int current = 0;
        for (const unsigned char c: word) {
            if (pool[current].children[c] == -1) {
                pool[current].children[c] = pool.size();
                pool[current].explicitChild[c] = true;
                pool.emplace_back();
            }
            current = pool[current].children[c];
        }
        pool[current].vocab_index = index;
    }

    FORCE_INLINE void build() {
        std::queue<TRIE_INTEGER> q;
        for (TRIE_INTEGER c = 0; c < 256; ++c) {
            if (TRIE_INTEGER child = pool[0].children[c]; child != -1) {
                pool[child].fail = 0;
                q.push(child);
            } else {
                pool[0].children[c] = 0;
                pool[0].explicitChild[c] = false;
            }
        }
        while (!q.empty()) {
            const TRIE_INTEGER cur = q.front();
            q.pop();
            const TRIE_INTEGER f = pool[cur].fail;
            for (TRIE_INTEGER c = 0; c < 256; ++c) {
                if (TRIE_INTEGER child = pool[cur].children[c]; child != -1 && pool[cur].explicitChild[c]) {
                    pool[child].fail = pool[f].children[c];
                    q.push(child);
                } else {
                    pool[cur].children[c] = pool[f].children[c];
                }
            }
        }
    }


    [[nodiscard]] std::pair<size_t, int> search(const std::string &token, const size_t start) const {
        TRIE_INTEGER current = 0;
        size_t best_length = 0;
        TRIE_INTEGER best_index = -1;
        for (size_t pos = start; pos < token.size(); ++pos) {
            const unsigned char c = token[pos];
            if (!this->pool[current].explicitChild[c])
                break;
            current = pool[current].children[c];
            if (pool[current].vocab_index != -1) {
                best_length = pos - start + 1;
                best_index = this->pool[current].vocab_index;
            }
        }
        return {best_length, best_index};
    }
};

#else

class Trie {
protected:
    std::vector<TrieNode> pool;

public:
    Trie() {
        pool.reserve(1024);
        pool.emplace_back();
    }

    FORCE_INLINE void insert(const std::string &word, int index) {
        int current = 0;
        for (const unsigned char c: word) {
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

    [[nodiscard]] std::pair<size_t, int> search(const std::string &token, const size_t start) const {
        TRIE_INTEGER current = 0;
        size_t best_length = 0;
        TRIE_INTEGER best_index = -1;
        for (size_t pos = start; pos < token.size(); ++pos) {
            const unsigned char c = token[pos];
            const TRIE_INTEGER child = this->pool[current].children[c];
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

    FORCE_INLINE void build() {
    }
};

#endif

#endif
