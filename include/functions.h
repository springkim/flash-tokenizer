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
#ifndef LVCZSHPWCVMPWCPYHRKYHEJBSZRTVAXCMPUQFCFWFSTTETZCMVYCJYDVVSSNZS
#define LVCZSHPWCVMPWCPYHRKYHEJBSZRTVAXCMPUQFCFWFSTTETZCMVYCJYDVVSSNZS
#include "defines.h"
#include <cctype>
#include <codecvt>
#include <locale>


#include <robin_hood.h>
#include <array>

namespace {
    constexpr std::array<bool, 65520> create_punctuation_array() {
        std::array<bool, 65520> arr{};

        for (int i = 33; i <= 47; i++) arr[i] = true;
        for (int i = 58; i <= 64; i++) arr[i] = true;
        for (int i = 91; i <= 96; i++) arr[i] = true;
        for (int i = 123; i <= 126; i++) arr[i] = true;

        for (int i = 0x2000; i <= 0x206F; i++) arr[i] = true;
        for (int i = 0x3000; i <= 0x303F; i++) arr[i] = true;
        for (int i = 0xFF00; i <= 0xFFEF; i++) arr[i] = true;
        for (int i = 0xFE30; i <= 0xFE4F; i++) arr[i] = true;

        constexpr int special_punct[] = {
            0x201C, 0x201D, 0x2018, 0x2019, 0x300C, 0x300D,
            0x300E, 0x300F, 0xFF5F, 0xFF60, 0x2E80, 0x2E99,
            0x2E9B, 0x2EF3, 0x2028, 0x2029
        };
        for (int cp: special_punct) arr[cp] = true;

        return arr;
    }

    constexpr std::array<bool, 8293> create_control_array() {
        std::array<bool, 8293> arr{};

        for (int i = 0; i <= 0x1F; i++) {
            if (i != 0x09 && i != 0x0A && i != 0x0D) {
                arr[i] = true;
            }
        }

        for (int i = 0x7F; i <= 0x9F; i++) arr[i] = true;

        for (int i = 0x200B; i <= 0x200F; i++) arr[i] = true;
        for (int i = 0x202A; i <= 0x202E; i++) arr[i] = true;
        for (int i = 0x2060; i <= 0x2064; i++) arr[i] = true;

        return arr;
    }

    constexpr std::array<bool, 12289> create_whitespace_array() {
        std::array<bool, 12289> arr{};

        arr[0x20] = true;
        arr[0x09] = true;
        arr[0x0A] = true;
        arr[0x0D] = true;

        constexpr int unicode_spaces[] = {
            0xA0, 0x1680, 0x2000, 0x2001, 0x2002, 0x2003,
            0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009,
            0x200A, 0x202F, 0x205F, 0x3000
        };
        for (int cp: unicode_spaces) arr[cp] = true;

        return arr;
    }

    constexpr auto PUNCTUATION_ARRAY = create_punctuation_array();
    constexpr auto CONTROL_ARRAY = create_control_array();
    constexpr auto WHITESPACE_ARRAY = create_whitespace_array();
}

inline bool is_punctuation_cp(const int cp) {
    return cp < PUNCTUATION_ARRAY.size() && PUNCTUATION_ARRAY[cp];
}

inline bool is_control_cp(const int cp) {
    return cp < CONTROL_ARRAY.size() && CONTROL_ARRAY[cp];
}

inline bool is_whitespace_cp(const int cp) {
    return cp < WHITESPACE_ARRAY.size() && WHITESPACE_ARRAY[cp];
}
inline size_t utf8_char_length(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

inline int utf8_to_codepoint(std::string_view str, size_t pos) {
    const size_t len = str.size();
    if (pos >= len) return 0;

    const auto *bytes = reinterpret_cast<const unsigned char *>(str.data() + pos);
    const unsigned char first_byte = bytes[0];

    if (first_byte < 128) {
        return first_byte;
    }

    if ((first_byte & 0xE0) == 0xC0) {
        if (pos + 1 >= len) return 0;
        if ((bytes[1] & 0xC0) != 0x80) return 0;

        int cp = ((first_byte & 0x1F) << 6) | (bytes[1] & 0x3F);
        if (cp < 0x80) return 0;
        return cp;
    }

    if ((first_byte & 0xF0) == 0xE0) {
        if (pos + 2 >= len) return 0;
        if ((bytes[1] & 0xC0) != 0x80 || (bytes[2] & 0xC0) != 0x80) return 0;

        int cp = ((first_byte & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
        if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) return 0;
        return cp;
    }

    if ((first_byte & 0xF8) == 0xF0) {
        if (pos + 3 >= len) return 0;

        const unsigned char b1 = bytes[1];
        const unsigned char b2 = bytes[2];
        const unsigned char b3 = bytes[3];

        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return 0;

        int cp = ((first_byte & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
        if (cp < 0x10000 || cp > 0x10FFFF) return 0;
        return cp;
    }

    return 0;
}

inline std::string to_lower_case(const std::string_view &text) {
    std::string result;
    result.reserve(text.length());
    for (size_t i = 0; i < text.size();) {
        const int cp = utf8_to_codepoint(text, i);
        const size_t char_len = utf8_char_length(text[i]);
        if (cp < 128) {
            result += std::tolower(text[i]);
        } else {
            result.append(text.data() + i, char_len);
        }
        i += char_len;
    }
    return result;
}

inline std::string run_strip_accents(const std::string_view &text) {
    std::string output;
    output.reserve(text.length());
    for (size_t i = 0; i < text.size();) {
        const int cp = utf8_to_codepoint(text, i);
        const size_t char_len = utf8_char_length(text[i]);

        if ((cp >= 0xC0 && cp <= 0xD6) || (cp >= 0xD8 && cp <= 0xDD)) {
            output += 'A' + (cp - 0xC0) % 32;
        } else if ((cp >= 0xE0 && cp <= 0xF6) || (cp >= 0xF8 && cp <= 0xFD)) {
            output += 'a' + (cp - 0xE0) % 32;
        } else if (cp == 0xFF) {
            output += 'y';
        } else {
            output.append(text.data() + i, char_len);
        }

        i += char_len;
    }
    return output;
}

inline bool is_chinese_char(const int &cp) {
    if (cp >= 0x4E00 && cp <= 0x9FFF) return true;
    if (cp >= 0x3400 && cp <= 0x4DBF) return true;
    if (cp >= 0xF900 && cp <= 0xFAFF) return true;

    if (cp >= 0x20000 && cp <= 0x2A6DF) return true;
    if (cp >= 0x2A700 && cp <= 0x2B73F) return true;
    if (cp >= 0x2B740 && cp <= 0x2B81F) return true;
    if (cp >= 0x2B820 && cp <= 0x2CEAF) return true;
    if (cp >= 0x2F800 && cp <= 0x2FA1F) return true;
    return false;
}


[[nodiscard]] static std::string tokenize_chinese_chars(const std::string &text) {
    bool has_chinese = false;
    for (size_t i = 0; i < text.size();) {
        if (const int &cp = utf8_to_codepoint(text, i); is_chinese_char(cp)) {
            has_chinese = true;
            break;
        }
        i += utf8_char_length(text[i]);
    }
    if (!has_chinese) {
        return text;
    }
    std::string output;

    for (size_t i = 0; i < text.size();) {
        const int cp = utf8_to_codepoint(text, i);
        const size_t char_len = utf8_char_length(text[i]);
        if (!is_chinese_char(cp)) {
            output.append(text.substr(i, char_len));
        } else {
            output += ' ';
            output.append(text.substr(i, char_len));
            output += ' ';
        }
        i += char_len;
    }
    return output;
}


[[nodiscard]] static std::string clean_text(const std::string &text) {
    std::string output;

    for (size_t i = 0; i < text.size();) {
        const int cp = utf8_to_codepoint(text, i);
        const size_t char_len = utf8_char_length(text[i]);
        if (cp == 0 || cp == 0xfffd || cp == 0x2028 || cp == 0x2029 || is_control_cp(cp)) {
            i += char_len;
            continue;
        }
        if (!is_whitespace_cp(cp)) {
            output.append(text.substr(i, char_len));
        } else {
            output += ' ';
        }
        i += char_len;
    }
    return output;
}

template<typename StringList=STRING_LIST_FAST>
static StringList run_split_on_punc_do_lower(const std::string &text) {
    std::string &&lower_text = to_lower_case(text);
    std::string &&stripped = run_strip_accents(lower_text);
    bool has_punctuation = false;
    for (size_t i = 0; i < stripped.size();) {
        if (const int cp = utf8_to_codepoint(stripped, i); is_punctuation_cp(cp)) {
            has_punctuation = true;
            break;
        }
        i += utf8_char_length(stripped[i]);
    }
    StringList output;
    if (!has_punctuation) {
        output.emplace_back(stripped);
        return output;
    }

    size_t word_start = 0;
    bool in_word = false;
    for (size_t i = 0; i < stripped.size();) {
        const int cp = utf8_to_codepoint(stripped, i);
        size_t char_len = utf8_char_length(stripped[i]);

        if (is_punctuation_cp(cp)) {
            if (in_word) {
                output.push_back(stripped.substr(word_start, i - word_start));
                in_word = false;
            }
            output.push_back(stripped.substr(i, char_len));
        } else {
            if (!in_word) {
                word_start = i;
                in_word = true;
            }
        }

        i += char_len;
    }

    if (in_word) {
        output.push_back(stripped.substr(word_start));
    }
    return output;
}

template<typename StringList = STRING_LIST_FAST>
static StringList run_split_on_punc(const std::string &text) {
    size_t sz = text.size();
    size_t i = 0;
    bool has_punc = false;
    while (i < sz) {
        int cp = utf8_to_codepoint(text, i);
        size_t len = utf8_char_length(text[i]);
        if (is_punctuation_cp(cp)) {
            has_punc = true;
            break;
        }
        i += len;
    }
    StringList output;
    if (!has_punc) {
        output.emplace_back(text);
        return output;
    }
    i = 0;
    size_t word_start = 0;
    bool in_word = false;
    while (i < sz) {
        int cp = utf8_to_codepoint(text, i);
        size_t len = utf8_char_length(text[i]);
        if (is_punctuation_cp(cp)) {
            if (in_word) {
                output.emplace_back(text.substr(word_start, i - word_start));
                in_word = false;
            }
            output.emplace_back(text.substr(i, len));
        } else {
            if (!in_word) {
                word_start = i;
                in_word = true;
            }
        }
        i += len;
    }
    if (in_word)
        output.emplace_back(text.substr(word_start));
    return output;
}
#endif
