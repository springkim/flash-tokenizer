//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                                BasicTokenizer
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
#ifndef A509LU8VZQRP3PNW02Q5UH286VBZ13PY7GBV9F767ALLP3P9P50WVH7NZL7B3YS6
#define A509LU8VZQRP3PNW02Q5UH286VBZ13PY7GBV9F767ALLP3P9P50WVH7NZL7B3YS6

#include "functions.h"
#include "defines.h"

class BasicTokenizer {
public:
    const bool do_lower_case;
    std::array<char, 256> char_map{};
    std::array<bool, 256> isSpace{};

    explicit BasicTokenizer(bool lower_case)
        : do_lower_case(lower_case) {
        for (int i = 0; i < 256; ++i) {
            if (i == 0) {
                char_map[i] = 0;
            } else if (i < 128) {
                char_map[i] = std::isspace(i) ? ' ' : i;
            } else {
                char_map[i] = i;
            }
        }
        constexpr std::string_view white_space = " \t\n\v\f\r";
        std::for_each(white_space.begin(), white_space.end(), [&](char c)-> void {
            this->isSpace[c] = true;
        });
    }

    template<typename StringList=STRING_LIST>
    [[nodiscard]] StringList tokenize(const std::string &text) const {
        using SplitFunction = StringList (*)(const std::string &);
        const SplitFunction split_function = this->do_lower_case ? to_lower_split_on_punc<StringList> : split_on_punc<StringList>;

        const std::string cleaned = clean_text(text, this->char_map);
        const std::string tokenized = tokenize_chinese_chars(cleaned);
        const auto orig_tokens = whitespace_tokenize(tokenized, this->isSpace);
        StringList output;
        for (const auto &token: orig_tokens) {
            auto sub_tokens = split_function(token);
            CONCAT(output, sub_tokens);
        }
        return output;
    }
};
#endif
