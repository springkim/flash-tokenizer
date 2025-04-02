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

class ACTrie;

class TrieNode {
    std::array<int, 256> children{};
    std::array<bool, 256> explicitChild{};
    int vocab_index = -1;
    int fail = 0;

public:
    TrieNode() {
        children.fill(-1);
        explicitChild.fill(false);
    }

    friend class ACTrie;
};

class ACTrie {
    std::vector<TrieNode> pool;
    std::vector<int> dfa; // DFA(Deterministic Finite Automaton)
    std::vector<bool> isExplicit;

public:
    ACTrie() {
        pool.emplace_back();
    }

    void insert(const std::string &word, const int index) {
        int current = 0;
        for (const unsigned char c: word) {
            if (pool[current].children[c] == -1) {
                pool[current].children[c] = static_cast<int>(pool.size());
                pool[current].explicitChild[c] = true;
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
                pool[0].explicitChild[c] = false;
            }
        }
        while (!q.empty()) {
            const int cur = q.front();
            q.pop();
            const int f = pool[cur].fail;
            for (int c = 0; c < 256; ++c) {
                if (pool[cur].children[c] != -1 && pool[cur].explicitChild[c]) {
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
                isExplicit[state * 256 + c] = pool[state].explicitChild[c];
            }
        }
    }

    [[nodiscard]] std::pair<int, int> search(const std::string &token, const int start) const {
        int current = 0;
        int best_length = 0;
        int best_index = -1;
        const int size = static_cast<int>(token.size());
        for (int pos = start; pos < size; ++pos) {
            const unsigned char c = token[pos];
            const int index = current * 256 + c;
            const int next = dfa[index];
            if (!isExplicit[index])
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


using Trie = ACTrie;

#endif
