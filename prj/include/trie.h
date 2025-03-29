//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                         Aho-Corasick Trie(LinMaxMatching)
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
#ifndef REMOVEEXISTINGLEGACYCODEANDUSEONLYAHOCORASICKTRIE
#define REMOVEEXISTINGLEGACYCODEANDUSEONLYAHOCORASICKTRIE


#include <vector>
#include <array>
#include <string>
#include <queue>
#include <bitset>

class Trie;

class TrieNode {
    std::array<int, 256> children;
    std::bitset<256> explicitChild;
    int vocab_index = -1;
    int fail = 0;

public:
    TrieNode() {
        children.fill(-1);
        explicitChild.reset();
    }

    friend class Trie;
};

class Trie {
    std::vector<TrieNode> pool;
    std::vector<int> dfa; // DFA(Deterministic Finite Automaton)
    std::vector<bool> isExplicit;

public:
    Trie() {
        pool.emplace_back();
    }

    void insert(const std::string &word, const int index) {
        int current = 0;
        for (const unsigned char c: word) {
            if (pool[current].children[c] == -1) {
                pool[current].children[c] = pool.size();
                pool[current].explicitChild.set(c);
                pool.emplace_back();
            }
            current = pool[current].children[c];
        }
        pool[current].vocab_index = index;
    }

    void build() {
        std::queue<int> q;
        for (int c = 0; c < 256; ++c) {
            if (pool[0].children[c] != -1) {
                pool[pool[0].children[c]].fail = 0;
                q.push(pool[0].children[c]);
            } else {
                pool[0].children[c] = 0;
                pool[0].explicitChild.reset(c);
            }
        }
        while (!q.empty()) {
            const int cur = q.front();
            q.pop();
            const int f = pool[cur].fail;
            for (int c = 0; c < 256; ++c) {
                if (pool[cur].children[c] != -1 && pool[cur].explicitChild.test(c)) {
                    int child = pool[cur].children[c];
                    pool[child].fail = pool[f].children[c];
                    q.push(child);
                } else {
                    pool[cur].children[c] = pool[f].children[c];
                }
            }
        }
        dfa.resize(pool.size() * 256, -1);
        isExplicit.resize(pool.size() * 256, false);
        for (size_t state = 0; state < pool.size(); state++) {
            for (int c = 0; c < 256; c++) {
                dfa[state * 256 + c] = pool[state].children[c];
                isExplicit[state * 256 + c] = pool[state].explicitChild.test(c);
            }
        }
    }

    [[nodiscard]] std::pair<size_t, int> search(const std::string &token, const size_t start) const {
        int current = 0;
        size_t best_length = 0;
        int best_index = -1;
        for (size_t pos = start; pos < token.size(); pos++) {
            const unsigned char c = token[pos];
            const int next = dfa[current * 256 + c];
            if (!isExplicit[current * 256 + c])
                break;
            current = next;
            if (pool[current].vocab_index != -1) {
                best_length = pos - start + 1;
                best_index = pool[current].vocab_index;
            }
        }
        return {best_length, best_index};
    }
};


// #include <vector>
// #include <array>
// #include <string>
// #include <queue>
// #include <bitset>
// class Trie;
//
//
// class TrieNode {
//     std::array<int, 256> children{};
//
//     std::array<bool, 256> explicitChild{}; // std::bitset<256> explicitChild{}; 으로 바꿔줘
//     int vocab_index = -1;
//     int fail = 0;
//
// public:
//     TrieNode() {
//         children.fill(-1);
//         explicitChild.fill(false);
//     }
//
//     friend Trie;
// };
//
//
// class Trie {
//     std::vector<TrieNode> pool{};
//
// public:
//     Trie() {
//         pool.emplace_back();
//     }
//
//     FORCE_INLINE void insert(const std::string &word, const int index) {
//         int current = 0;
//         for (const unsigned char c: word) {
//             if (pool[current].children[c] == -1) {
//                 pool[current].children[c] = static_cast<int>(this->pool.size());
//                 pool[current].explicitChild[c] = true;
//                 pool.emplace_back();
//             }
//             current = pool[current].children[c];
//         }
//         pool[current].vocab_index = index;
//     }
//
//     FORCE_INLINE void build() {
//         std::queue<int> q;
//         for (int c = 0; c < 256; ++c) {
//             if (int child = this->pool[0].children[c]; child != -1) {
//                 this->pool[child].fail = 0;
//                 q.push(child);
//             } else {
//                 this->pool[0].children[c] = 0;
//                 this->pool[0].explicitChild[c] = false;
//             }
//         }
//         while (!q.empty()) {
//             const int cur = q.front();
//             q.pop();
//             const int f = this->pool[cur].fail;
//             for (int c = 0; c < 256; ++c) {
//                 if (int child = this->pool[cur].children[c]; child != -1 && pool[cur].explicitChild[c]) {
//                     this->pool[child].fail = this->pool[f].children[c];
//                     q.push(child);
//                 } else {
//                     this->pool[cur].children[c] = this->pool[f].children[c];
//                 }
//             }
//         }
//     }
//
//
//     [[nodiscard]] std::pair<size_t, int> search(const std::string &token, const size_t start) const {
//         int current = 0;
//         size_t best_length = 0;
//         int best_index = -1;
//
//         for (size_t pos = start; pos < token.size(); ++pos) {
//             if (const unsigned char c = token[pos]; this->pool[current].explicitChild[c]) {
//                 current = this->pool[current].children[c];
//                 if (this->pool[current].vocab_index != -1) {
//                     best_length = pos - start + 1;
//                     best_index = this->pool[current].vocab_index;
//                 }
//             } else {
//                 break;
//             }
//         }
//         return {best_length, best_index};
//     }
// };


#endif
