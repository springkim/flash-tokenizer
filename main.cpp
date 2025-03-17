#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <chrono>
#include <string>
#include <thread>
#include <future>
#include "timer.h"

#include "bert_tokenizer.h"
#include "env.h"


#define DATASET_NUMBER 6

#if DATASET_NUMBER == 1
// KCBERT_BASE
#define KCBERT_BASE
#elif DATASET_NUMBER == 2
//DEEPCT
#define DEEPCT
#elif DATASET_NUMBER == 3
//DEEPCT_KRBERT
#define DEEPCT_KRBERT
#elif DATASET_NUMBER == 4
//SPLADE
#define SPLADE
#elif DATASET_NUMBER == 5
#define SPLADE_NORMAL
#elif DATASET_NUMBER == 6
#define SPLADE_UNIFORM
#endif

#define MP 1

#ifdef KCBERT_BASE
#define TEXTS_PATH "../dataset/kcbert_base/text_10M.txt"
#define IDS_PATH "../dataset/kcbert_base/text_10M_gt.txt"
#define VOCAB_PATH "../dataset/kcbert_base/vocab_kcbert_base.txt"
#define MAX_LENGTH 300
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
std::string DATASET_NAME="kcbert_base";
#endif

#ifdef DEEPCT
#define TEXTS_PATH "../dataset/deepct/titles_404464.txt"
#define IDS_PATH "../dataset/deepct/gt_404464.txt"
#define VOCAB_PATH "../dataset/deepct/vocab_char_16424.txt"
#define MAX_LENGTH 256
#define DO_LOWER true
using TokenizerClass = FlashBertTokenizer;
std::string DATASET_NAME = "deepct(w/o bidirectional)";
#endif

#ifdef DEEPCT_KRBERT
#define TEXTS_PATH "../dataset/deepct_kobert/titles_404464.txt"
#define IDS_PATH "../dataset/deepct_kobert/titles_404464_ids.txt"
#define VOCAB_PATH "../dataset/deepct_kobert/vocab_char_16424.txt"
#define MAX_LENGTH 256
#define DO_LOWER true
using TokenizerClass = FlashBertTokenizerBidirectional;
std::string DATASET_NAME = "deepct(bidirectional)";
#endif

#ifdef SPLADE
#define TEXTS_PATH "../dataset/splade/titles_404464.txt"
#define IDS_PATH "../dataset/splade/titles_404464_gt.txt"
#define VOCAB_PATH "../dataset/splade/vocab.txt"
#define MAX_LENGTH 512
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
std::string DATASET_NAME="splade";
#endif

#ifdef SPLADE_NORMAL
#define TEXTS_PATH "../dataset/splade/calibration_normal.txt"
#define IDS_PATH "../dataset/splade/calibration_normal_gt.txt"
#define VOCAB_PATH "../dataset/splade/vocab.txt"
#define MAX_LENGTH 512
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
std::string DATASET_NAME="splade_normal";
#endif

#ifdef SPLADE_UNIFORM
#define TEXTS_PATH "../dataset/splade/calibration_uniform.txt"
#define IDS_PATH "../dataset/splade/calibration_uniform_gt.txt"
#define VOCAB_PATH "../dataset/splade/vocab.txt"
#define MAX_LENGTH 512
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
std::string DATASET_NAME="splade_uniform";
#endif

using namespace std;


vector<string> load_titles() {
    std::ifstream file(TEXTS_PATH, std::ios::binary | std::ios::ate);
    if (!file) return {};
    std::streamsize size = file.tellg();
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

std::vector<int> parseNumbersFromString(const std::string_view input) {
    std::vector<int> numbers;
    numbers.reserve(100);
    const char *str = input.data();
    const char *const end = str + input.length();
    const char *open_bracket = static_cast<const char *>(memchr(str, '[', end - str));
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

vector<vector<int> > load_gt() {
    std::ifstream file(IDS_PATH, std::ios::binary | std::ios::ate);
    if (!file) return {};
    std::streamsize size = file.tellg();
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


void test() {
#define LOAD_PARALLEL 1
    Timer::Tick("LoadDataset");
#if LOAD_PARALLEL == 1
    std::future<std::vector<std::string> > titles_future =
            std::async(std::launch::async, load_titles);

    std::future<std::vector<std::vector<int> > > gts_future =
            std::async(std::launch::async, load_gt);
    auto texts = titles_future.get();
    auto gts = gts_future.get();
    double accuracy;
    double min_elapsed_time=std::numeric_limits<double>::max();
#else
    auto texts = load_titles();
    auto gts = load_gt();
#endif
    Timer::Tock("LoadDataset");
    cout << "Data/GT loaded : " << Timer::Watch("LoadDataset").accu << endl;
    cout << DATASET_NAME << endl;
    TokenizerClass tokenizer(VOCAB_PATH, DO_LOWER);
    for (int testcase=0;testcase<3;testcase++) {
        std::chrono::system_clock::time_point t_beg, t_end;
        std::chrono::duration<double> diff{};

        t_beg = std::chrono::system_clock::now();

        size_t correct = 0;

#if MP!=1
        cout << "BatchedEncoding(Multi Processing)" << " - " << MP << endl;
        vector<vector<string> > titles;
        vector<vector<vector<int> > > gts_group;
        size_t num_chunks = std::ceil((texts.size() + MP - 1.0) / MP);
        titles.reserve(num_chunks);
        gts_group.reserve(num_chunks);
        for (size_t i = 0; i < texts.size(); i += MP) {
            size_t chunk_size = std::min(MP, static_cast<int>(texts.size() - i));
            vector<string> title_chunk;
            vector<vector<int> > gt_chunk;
            title_chunk.reserve(chunk_size);
            gt_chunk.reserve(chunk_size);
            for (size_t j = 0; j < chunk_size; j++) {
                title_chunk.push_back(std::move(texts[i + j]));
                gt_chunk.push_back(std::move(gts[i + j]));
            }
            titles.push_back(std::move(title_chunk));
            gts_group.push_back(std::move(gt_chunk));
        }
        for (size_t i = 0; i < titles.size(); i++) {
            auto ids = tokenizer.batch_encode(titles[i], "longest", MAX_LENGTH);

            for (size_t j = 0; j < ids.size(); j++) {
                if (ids[j] == gts_group[i][j]) {
                    correct += 1;
                }
            }
        }
#else
        cout << "SingleEncoding(Single Thread)" << endl;
        for (size_t i = 0; i < texts.size(); i++) {
            auto ids = tokenizer.encode(texts[i], "longest", MAX_LENGTH);
            correct += ids == gts[i];
        }
#endif


        t_end = std::chrono::system_clock::now();
        diff = t_end - t_beg;
        auto elapsed_time = diff.count();
        std::cout << elapsed_time << " seconds" << "  |  ";

        min_elapsed_time = std::min(min_elapsed_time, elapsed_time);
        std::cout << texts.size() << "  |  ";
        accuracy = static_cast<double>(correct) * 100.0 / static_cast<double>(texts.size());
        std::cout << accuracy << " % Accuracy" << std::endl;
        std::cout << static_cast<double>(texts.size()) / elapsed_time << " RPS" << std::endl;
        std::cout << "--------------" << std::endl;
    }
    cout << "|" << DATASET_NAME << "|" << min_elapsed_time << "|" << accuracy << "|" << std::endl;
}


int main() {
    std::ios::sync_with_stdio(false);
    cout << cpp_env() << endl;
    test();

    cout << g_1 << "\t" << g_2 << endl;
    return 0;
}