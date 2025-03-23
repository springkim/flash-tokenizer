#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <string>
#include "bert_tokenizer.h"
#include "env.h"
#include "debugging.h"


#include<json.hpp> // https://json.nlohmann.me/api/json/

using json = nlohmann::json;
using namespace std;

struct DataList {
    static const std::string bert_base_cased;
    static const std::string bert_base_uncased;
    static const std::string bert_base_multilingual_cased;
    static const std::string kcbert_base;
    static const std::string KR_BERT;
    static const std::string llmlingua_2_bert_base_multilingual_cased_meetingbank;
};

const std::string DataList::bert_base_cased = "bert-base-cased";
const std::string DataList::bert_base_uncased = "bert-base-uncased";
const std::string DataList::bert_base_multilingual_cased = "bert-base-multilingual-cased";
const std::string DataList::kcbert_base = "kcbert-base";
const std::string DataList::KR_BERT = "KR-BERT";
const std::string DataList::llmlingua_2_bert_base_multilingual_cased_meetingbank = "llmlingua-2-bert-base-multilingual-cased-meetingbank";

class TestData {
public:
    std::string text_file;
    std::string list_file;
    std::string vocab_file;
    std::string tokenizer_config_file;
    bool do_lower_case{};
    int model_max_length{};
    std::vector<std::string> texts{};
    std::vector<std::vector<int> > gts{};
    FlashBertTokenizer *tokenizer = nullptr;

    ~TestData() {
        if (tokenizer != nullptr) {
            delete tokenizer;
        }
    }

    explicit TestData(const std::string &config) {
        std::ostringstream dirname;
        dirname << "../dataset/" << config << "/";
        this->text_file = dirname.str() + "texts.txt";
        this->list_file = dirname.str() + "ids.txt";
        this->vocab_file = dirname.str() + "vocab.txt";
        this->tokenizer_config_file = dirname.str() + "tokenizer_config.json";

        this->texts = load_text_lines(this->text_file);
        auto gts_tmp = load_text_lines(this->list_file);
        for (auto &gt: gts_tmp) {
            this->gts.push_back(parseNumbersFromString(gt));
        }

        this->load_tokenizer_config(this->tokenizer_config_file);
        if (config == "KR-BERT") {
            tokenizer = new FlashBertTokenizerBidirectional(this->vocab_file, this->do_lower_case);
        } else {
            tokenizer = new FlashBertTokenizer(this->vocab_file, this->do_lower_case);
        }
    }

    static std::vector<std::string> load_text_lines(const std::string &file_path) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file) return {};
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) return {};
        std::vector<std::string> lines;
        lines.reserve(size / 30);
        const char *start = buffer.data();
        const char *end = start + size;
        const char *ptr = start;
        while (ptr < end) {
            const char *lineEnd = std::find(ptr, end, '\n');
            std::string_view line(ptr, lineEnd - ptr);
            if (!line.empty() && line.back() == '\r')
                line.remove_suffix(1);
            lines.emplace_back(line);
            ptr = lineEnd < end ? lineEnd + 1 : end;
        }
        return lines;
    }

    static std::vector<int> parseNumbersFromString(const std::string_view input) {
        std::vector<int> numbers;
        numbers.reserve(100);
        const char *str = input.data();
        const char *const end = str + input.length();
        const auto open_bracket = static_cast<const char *>(memchr(str, '[', end - str));
        if (!open_bracket) return numbers;
        str = open_bracket + 1;

        while (str < end) {
            while (str < end) {
                if (*str == ']') return numbers;
                if ((*str >= '0' && *str <= '9') || *str == '-') break;
                ++str;
            }
            if (str >= end) break;
            bool negative = false;
            if (*str == '-') {
                negative = true;
                ++str;
            }
            int num = 0;
            while (str < end && *str >= '0' && *str <= '9') {
                num = (num << 3) + (num << 1) + (*str - '0');
                ++str;
            }

            numbers.push_back(negative ? -num : num);
        }

        return numbers;
    }

    static vector<vector<int> > load_gt(const std::string &file_path) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file) return {};
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) return {};
        std::vector<std::vector<int> > gts;
        gts.reserve(size / 50);
        const char *start = buffer.data();
        const char *end = start + size;
        const char *ptr = start;
        while (ptr < end) {
            const char *lineEnd = std::find(ptr, end, '\n');
            std::string_view line(ptr, lineEnd - ptr);
            if (!line.empty() && line.back() == '\r')
                line.remove_suffix(1);
            gts.push_back(parseNumbersFromString(line));
            ptr = lineEnd < end ? lineEnd + 1 : end;
        }
        return gts;
    }

    void load_tokenizer_config(const std::string &file_path) {
        std::ifstream file(file_path);
        const json data = json::parse(file);

        this->do_lower_case = data.value("do_lower_case", false);
        this->model_max_length = data.value("model_max_length", 512);
    }
};

void perf_test(bool parallel = false) {
    cout << "Start performance test..." << endl;
    const TestData td(DataList::llmlingua_2_bert_base_multilingual_cased_meetingbank);
    std::chrono::duration<double> diff{};
    size_t correct = 0;
    const auto t_beg = std::chrono::system_clock::now();
    vector<vector<int> > ids_list;
    if (parallel) {
        ids_list = td.tokenizer->batch_encode(td.texts, "longest", td.model_max_length);
    } else {
        for (size_t i = 0; i < td.texts.size(); i++) {
            auto ids = td.tokenizer->encode(td.texts[i], "longest", td.model_max_length);
            ids_list.emplace_back(ids);
        }
    }
    const auto t_end = std::chrono::system_clock::now();
    diff = t_end - t_beg;
    const auto elapsed_time = diff.count();

    for (size_t i = 0; i < td.texts.size(); i++) {
        correct += ids_list[i] == td.gts[i];
    }
    std::vector<std::string> thread_option = {"[ST]", "[MT]"};
    std::cout << thread_option[parallel] << " | " << elapsed_time << " seconds" << "  |  ";
    std::cout << td.texts.size() << "  |  ";
    const double accuracy = static_cast<double>(correct) * 100.0 / static_cast<double>(td.texts.size());
    std::cout << accuracy << " % Accuracy" << std::endl;
    std::cout << "--------------" << std::endl;
}

int main() {
    std::ios::sync_with_stdio(false);
    cout << cpp_env() << endl;
    perf_test(true);

    //cout << g_1 << "\t" << g_2 << "\t" << g_3 << "\t" << g_4 << endl;
    return 0;
}
