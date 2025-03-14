//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                               Tokenizer Functions
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
#ifndef X3fRGM1KSjAgYkHBEBjHGu38HLAHPKH1nZfPErLC4bfwb6NNueYNUN42KM5bCcPg
#define X3fRGM1KSjAgYkHBEBjHGu38HLAHPKH1nZfPErLC4bfwb6NNueYNUN42KM5bCcPg

#include "defines.h"
#include <array>


template<typename StringList=STRING_SEQUENCE>
OPTIMIZED static StringList whitespace_tokenize(const std::string &text, const std::array<bool, 256> &isSpace) {
    StringList tokens;
    const char *data = text.data();
    const char *end = data + text.size();
    while (data < end) {
        while (data < end && isSpace[static_cast<unsigned char>(*data)])
            ++data;

        if (data >= end)
            break;

        const char *start = data;
        while (data < end && !isSpace[static_cast<unsigned char>(*data)])
            ++data;
        tokens.emplace_back(start, data - start);
    }
    return tokens;
}


static std::string clean_text(const std::string &text, const std::array<char, 256> &char_map) {
    if (text.empty())
        return {};
    std::string result;
    result.reserve(text.size());
    const char *p = text.data(), *end = p + text.size();
    const char *cmap = char_map.data();

    while (p < end) {
        const auto c = static_cast<unsigned char>(*p);
        if (c == 0) {
            ++p;
            continue;
        }
        if (c > 0x80) {
            int skip = 1;
            if ((c & 0xE0) == 0xC0 && p + 1 < end) {
                skip = 2;
                result.push_back(p[0]);
                result.push_back(p[1]);
            } else if ((c & 0xF0) == 0xE0 && p + 2 < end) {
                skip = 3;
                result.push_back(p[0]);
                result.push_back(p[1]);
                result.push_back(p[2]);
            } else if ((c & 0xF8) == 0xF0 && p + 3 < end) {
                skip = 4;
                result.push_back(p[0]);
                result.push_back(p[1]);
                result.push_back(p[2]);
                result.push_back(p[3]);
            }
            p += skip;
        } else {
            while (p < end && static_cast<unsigned char>(*p) < 0x80 && *p != 0) {
                result.push_back(cmap[static_cast<unsigned char>(*p++)]);
            }
        }
    }
    return result;
}

constexpr std::array<bool, 256> createAsciiPunctBloom() {
    std::array<bool, 256> filter{};
    for (size_t i = 0; i < filter.size(); ++i) {
        auto c = static_cast<unsigned char>(i);
        filter[i] = ((c >= 33 && c <= 47) ||
                     (c >= 58 && c <= 64) ||
                     (c >= 91 && c <= 96) ||
                     (c >= 123 && c <= 126));
    }
    return filter;
}

constexpr std::array<bool, 256> createAsciiSpaceBloom() {
    std::array<bool, 256> filter{};
    for (size_t i = 0; i < filter.size(); ++i) {
        auto c = static_cast<unsigned char>(i);
        filter[i] = (c == 32 || c == 9 || c == 10 || c == 11 || c == 12 || c == 13);
    }
    return filter;
}

constexpr auto asciiPunctBloom = createAsciiPunctBloom();
constexpr auto asciiSpaceBloom = createAsciiSpaceBloom();

template<typename StringList=STRING_LIST_FAST>
OPTIMIZED static StringList split_on_punc(const std::string &text) {
    StringList tokens;
    auto sv = std::string_view(text);
    const char *p = sv.data();
    const char *end = p + sv.size();
    const char *token_start = p;

    auto flush_token = [&]() {
        if (token_start < p)
            tokens.emplace_back(token_start, p - token_start);
    };

    while (p < end) {
        auto c = static_cast<unsigned char>(*p);
        if (c < 0x80) {
            if (asciiPunctBloom[c] || asciiSpaceBloom[c]) {
                flush_token();
                if (asciiPunctBloom[c])
                    tokens.emplace_back(p, 1);
                ++p;
                token_start = p;
            } else {
                ++p;
            }
        } else {
            int len = 1;
            if ((c & 0xE0) == 0xC0)
                len = 2;
            else if ((c & 0xF0) == 0xE0)
                len = 3;
            else if ((c & 0xF8) == 0xF0)
                len = 4;
            p += len;
        }
    }
    flush_token();
    return std::move(tokens);
}

template<typename StringList=STRING_LIST_FAST>
OPTIMIZED static StringList to_lower_split_on_punc(const std::string &text) {
    StringList tokens;
    std::string buffer;
    const char *p = text.data(), *end = p + text.size();
    while (p < end) {
        auto c = static_cast<unsigned char>(*p);
        if (c < 0x80) {
            if (char lc = (c >= 'A' && c <= 'Z') ? c + 32 : c; std::ispunct(static_cast<unsigned char>(lc))) {
                if (!buffer.empty()) {
                    tokens.push_back(std::move(buffer));
                    buffer.clear();
                }
                tokens.emplace_back(1, lc);
                ++p;
            } else if (std::isspace(static_cast<unsigned char>(lc))) {
                if (!buffer.empty()) {
                    tokens.push_back(std::move(buffer));
                    buffer.clear();
                }
                ++p;
            } else {
                buffer.push_back(lc);
                ++p;
            }
        } else {
            const int len = (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 1;
            if (p + len > end) break;
            buffer.append(p, len);
            p += len;
        }
    }
    if (!buffer.empty())
        tokens.push_back(std::move(buffer));
    return tokens;
}

class ChineseCharDetector {
private:
    struct Range {
        uint32_t start, end;

        bool operator<(const Range &other) const {
            return end < other.start;
        }
    };

    static const std::vector<Range> ranges;

public:
    static bool isChineseChar(uint32_t codepoint) {
        if (codepoint < 0x3400 || codepoint > 0x2FA1F)
            return false;

        return std::any_of(ranges.begin(), ranges.end(), [codepoint](const auto &range) {
            return codepoint >= range.start && codepoint <= range.end;
        });
    }
};

SELECT_ANY const std::vector<ChineseCharDetector::Range> ChineseCharDetector::ranges = {
    {0x4E00, 0x9FFF},
    {0x3400, 0x4DBF},
    {0xF900, 0xFAFF},
    {0x20000, 0x2A6DF},
    {0x2A700, 0x2B73F},
    {0x2B740, 0x2B81F},
    {0x2B820, 0x2CEAF},
    {0x2F800, 0x2FA1F}
};


static std::string tokenize_chinese_chars(const std::string &text) {
    std::string output;
    //output.reserve(text.size());
    size_t i = 0;
    size_t n = text.size();
    while (i < n) {
        unsigned char c = text[i];
        uint32_t codepoint = c;
        int bytes = 1;
        if (c >= 0x80) {
            const int len = (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : (c < 0xF8) ? 4 : 0;
            if (!len || i + len > n) {
                break;
            }
            if (len == 2) {
                codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(text[i + 1]) & 0x3F);
            } else if (len == 3) {
                codepoint = ((c & 0x0F) << 12) |
                            ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(text[i + 2]) & 0x3F);
            } else if (len == 4) {
                codepoint = ((c & 0x07) << 18) |
                            ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                            ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(text[i + 3]) & 0x3F);
            }
            bytes = len;
        }
        if (!ChineseCharDetector::isChineseChar(codepoint)) {
            for (int k = 0; k < bytes; ++k)
                output.push_back(text[i + k]);
        } else {
            output.push_back(' ');
            for (int k = 0; k < bytes; ++k)
                output.push_back(text[i + k]);
            output.push_back(' ');
        }
        i += bytes;
    }
    return output;
}

#endif
