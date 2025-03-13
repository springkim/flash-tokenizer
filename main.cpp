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

#define DEEPCT
//#define  KCBERT_BASE
//#define DEEPCT_KRBERT
//#define SPLADE


//#define MP 256

#ifdef KCBERT_BASE
#define TEXTS_PATH "../dataset/kcbert_base/text_1M.txt"
#define IDS_PATH "../dataset/kcbert_base/text_1M_gt.txt"
#define VOCAB_PATH "../dataset/kcbert_base/vocab_kcbert_base.txt"
#define MAX_LENGTH 300
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
#endif

#ifdef DEEPCT
#define TEXTS_PATH "../dataset/deepct/titles_404464.txt"
#define IDS_PATH "../dataset/deepct/gt_404464.txt"
#define VOCAB_PATH "../dataset/deepct/vocab_char_16424.txt"
#define MAX_LENGTH 256
#define DO_LOWER true
using TokenizerClass = FlashBertTokenizer;
#endif

#ifdef DEEPCT_KRBERT
#define TEXTS_PATH "../dataset/deepct_kobert/titles_404464.txt"
#define IDS_PATH "../dataset/deepct_kobert/titles_404464_ids.txt"
#define VOCAB_PATH "../dataset/deepct_kobert/vocab_char_16424.txt"
#define MAX_LENGTH 256
#define DO_LOWER true
using TokenizerClass = FlashBertTokenizerBidirectional;
#endif

#ifdef SPLADE
#define TEXTS_PATH "../dataset/splade/titles_404464.txt"
#define IDS_PATH "../dataset/splade/titles_404464_gt.txt"
#define VOCAB_PATH "../dataset/splade/vocab.txt"
#define MAX_LENGTH 512
#define DO_LOWER false
using TokenizerClass = FlashBertTokenizer;
#endif

using namespace std;

std::vector<int> parseNumbersFromString(const std::string &input) {
    std::vector<int> numbers;
    numbers.reserve(100);
    const char *str = input.c_str();
    const char *end = str + input.length();
    while (str < end && *str != '[') str++;
    if (str < end) str++;
    while (str < end) {
        while (str < end && (*str < '0' || *str > '9') && *str != '-') {
            if (*str == ']') return numbers;
            str++;
        }
        if (str >= end) break;
        bool negative = false;
        if (*str == '-') {
            negative = true;
            str++;
        }
        int num = 0;
        while (str < end && *str >= '0' && *str <= '9') {
            num = num * 10 + (*str - '0');
            str++;
        }
        numbers.push_back(negative ? -num : num);
    }
    return numbers;
}

deque<string> load_titles() {
    std::fstream fin(TEXTS_PATH, std::ios::in);
    std::deque<std::string> lines;
    std::string line;
    while (getline(fin, line)) {
        if (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
            line.pop_back();
        lines.push_back(line);
    }
    fin.close();
    std::cout << "Data loaded!!" << std::endl;
    return lines;
}

deque<vector<int> > load_gt() {
    std::fstream fin(IDS_PATH, std::ios::in);
    std::deque<std::vector<int> > gts;
    std::string gt;
    while (std::getline(fin, gt)) {
        gts.push_back(parseNumbersFromString(gt));
    }
    fin.close();
    std::cout << "GT loaded!!" << std::endl;
    return gts;
}


void test() {
#define LOAD_PARALLEL 1
    Timer::Tick("LoadDataset");
#if LOAD_PARALLEL == 0
    auto texts = load_titles();
    auto gts = load_gt();
#else
    std::future<std::deque<std::string> > titles_future =
            std::async(std::launch::async, load_titles);

    std::future<std::deque<std::vector<int> > > gts_future =
            std::async(std::launch::async, load_gt);
    auto texts = titles_future.get();
    auto gts = gts_future.get();
#endif
    Timer::Tock("LoadDataset");
    cout << "Loading: " << Timer::Watch("LoadDataset").accu << endl;


    FlashBertTokenizer tokenizer(VOCAB_PATH, DO_LOWER);
    std::chrono::system_clock::time_point t_beg, t_end;
    std::chrono::duration<double> diff{};

    t_beg = std::chrono::system_clock::now();

    size_t correct = 0;

#if defined(MP) && MP!=0
    cout << "BatchedEncoding(Multi Processing)" << endl;
    vector<vector<string> > titles;
    vector<vector<vector<int> > > gts_group;
    vector<string> chunk;
    vector<vector<int> > gt_chunk;
    for (size_t i = 0; i < texts.size(); i++) {
        chunk.push_back(texts[i]);
        gt_chunk.push_back(gts[i]);
        if (chunk.size() == MP) {
            titles.push_back(chunk);
            chunk.clear();
            gts_group.push_back(gt_chunk);
            gt_chunk.clear();
        }
    }
    gts_group.push_back(gt_chunk);
    titles.push_back(chunk);
    for (size_t i = 0; i < titles.size(); i++) {
        auto ids = tokenizer(titles[i], "longest", MAX_LENGTH);
        for (size_t j = 0; j < ids.size(); j++) {
            if (ids[j] == gts_group[i][j]) {
                correct += 1;
            }
        }
    }
#else
    for (size_t i = 0; i < texts.size(); i++) {
        auto ids = tokenizer(texts[i], "longest", MAX_LENGTH);
        correct += ids == gts[i];
    }
#endif


    t_end = std::chrono::system_clock::now();
    diff = t_end - t_beg;
    auto elapsed_time = diff.count();
    std::cout << elapsed_time << " seconds" << "\t";


    std::cout << texts.size() << "\t";
    std::cout << static_cast<double>(correct) * 100.0 / texts.size() << " % Accuracy" << std::endl;
    std::cout << static_cast<double>(texts.size()) / elapsed_time << " RPS" << std::endl;
    std::cout << "--------------" << std::endl;
}


void simple_test() {
    //wstring s=L"🙆‍";


    //string s = "김시온 대표 선경 인테리어 실무 시작, 미국 샌프란시스코 잠시 생활 2012년 한국 돌아와 스페이스 필모어 설립했다.";
    string s = "이것은 놀이공원인가, 호텔인가'…국내 최초 호텔에 테마파크 들어선다";
    // cout << convert_and_reverse(s) << endl;
    auto tokenizer = FlashBertTokenizer(VOCAB_PATH, false);
    auto ids = tokenizer(s, "longest", 512);
    for (auto &e: ids) {
        cout << e << ", ";
    }
    cout << endl;
    exit(0);
}


int main() {
    std::ios::sync_with_stdio(false);
    cout << cpp_env() << endl;
    //simple_test();
    test();
    return 0;
}
