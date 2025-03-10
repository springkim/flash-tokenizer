#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <chrono>


#include "bert_tokenizer.h"
#include "env.h"

//#define DEEPCT
#define  KCBERT_BASE

#ifdef KCBERT_BASE
#define TEXTS_PATH "../dataset/kcbert_base/text_10M.txt"
#define IDS_PATH "../dataset/kcbert_base/text_10M_gt.txt"
#define VOCAB_PATH "../dataset/kcbert_base/vocab_kcbert_base.txt"
#define MAX_LENGTH 300
#define DO_LOWER false
#endif

#ifdef DEEPCT
#define TEXTS_PATH "../dataset/deepct/titles_404464.txt"
#define IDS_PATH "../dataset/deepct/gt_404464.txt"
#define VOCAB_PATH "../dataset/deepct/vocab_char_16424.txt"
#define MAX_LENGTH 256
#define DO_LOWER true
#endif


using namespace std;

std::vector<int> parseNumbersFromString(const std::string &input) {
    std::vector<int> numbers;
    std::stringstream ss(input);
    char c;
    ss >> c;
    int num;
    while (ss >> num) {
        numbers.push_back(num);
        ss >> c;
        if (c == ']') break;
    }
    return numbers;
}

vector<string> load_titles() {
    std::fstream fin(TEXTS_PATH, std::ios::in);
    std::vector<std::string> lines;
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

vector<vector<int>> load_gt() {
    std::fstream fin(IDS_PATH, std::ios::in);
    std::vector<std::vector<int>> gts;
    std::string gt;
    while (std::getline(fin, gt)) {
        gts.push_back(parseNumbersFromString(gt));
    }
    fin.close();
    std::cout << "GT loaded!!" << std::endl;
    return gts;
}

void test() {
    auto texts = load_titles();
    auto gts = load_gt();

    FlashBertTokenizer tokenizer(VOCAB_PATH, DO_LOWER, MAX_LENGTH);
    std::chrono::system_clock::time_point t_beg, t_end;
    std::chrono::duration<double> diff{};

    t_beg = std::chrono::system_clock::now();

    int correct = 0;
    for (int i = 0; i < texts.size(); i++) {
        auto ids = tokenizer(texts[i], "longest", MAX_LENGTH);
        correct += ids == gts[i];
    }
    t_end = std::chrono::system_clock::now();
    diff = t_end - t_beg;
    auto elapsed_time = diff.count();
    std::cout << elapsed_time << " seconds" << "\t";

    std::cout << texts.size() << "\t";
    std::cout << static_cast<double>(correct) * 100.0 / texts.size() << " % Accuracy" << std::endl;
    std::cout << static_cast<double>(texts.size()) / elapsed_time << " RPS" << std::endl;
    std::cout << "--------------" << std::endl;

}

int main() {
    std::ios::sync_with_stdio(false);
    cout << cpp_env() << endl;

    test();
    return 0;
}