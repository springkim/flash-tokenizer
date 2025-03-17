#ifndef A509LU8VZQRP3PNW02Q5UH286VBZ13PY7GBV9F767ALLP3P9P50WVH7NZL7B3YS6
#define A509LU8VZQRP3PNW02Q5UH286VBZ13PY7GBV9F767ALLP3P9P50WVH7NZL7B3YS6


#include "defines.h"
#include "functions.h"

class BasicTokenizer {
public:
    const bool do_lower_case;

    explicit BasicTokenizer(bool lower_case = true)
        : do_lower_case(lower_case) {
    }

    template<typename StringList=STRING_LIST_FAST>
    [[nodiscard]] StringList tokenize(const std::string &text) const {
        using SplitFunction = StringList (*)(const std::string &);
        const SplitFunction split_function = this->do_lower_case ? run_split_on_punc_do_lower<StringList> : run_split_on_punc<StringList>;
        const std::string &cleaned = clean_text(text);
        const std::string &tokenized = tokenize_chinese_chars(cleaned);
        const auto orig_tokens = whitespace_tokenize<STRING_SEQUENCE>(tokenized);
        StringList output_tokens;
        for (const auto &token: orig_tokens) {
            StringList sub_tokens = split_function(token);
            CONCAT(output_tokens, sub_tokens);
        }
        return output_tokens;
    }

private:
    template<typename StringList=STRING_SEQUENCE>
    [[nodiscard]] StringList whitespace_tokenize(const std::string &text) const {
        StringList tokens;
        std::string token;
        bool in_token = false;
        for (const char c: text) {
            if (!is_whitespace_cp(c)) {
                in_token = true;
                token += c;
            } else {
                if (in_token) {
                    tokens.emplace_back(token);
                    token.clear();
                    in_token = false;
                }
            }
        }
        if (in_token && !token.empty()) {
            tokens.emplace_back(token);
        }
        return tokens;
    }
};


#endif
