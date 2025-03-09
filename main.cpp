#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "bert_tokenizer.h"
#include <set>
#include <thread>

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

std::vector<std::string> load_titles() {
    std::fstream fin("../res/titles_404464.txt", std::ios::in);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();
    std::cout << "Data loaded!!" << std::endl;
    return lines;
}

vector<vector<int>> load_gt() {
    std::fstream fin("../res/gt_404464.txt", std::ios::in);
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
    FlashBertTokenizer tokenizer("../res/vocab_char_16424.txt", true);


    auto lines = load_titles();
    auto gts = load_gt();


    for (int loop = 0; loop < 3; loop++) {
        std::chrono::system_clock::time_point t_beg, t_end;
        std::chrono::duration<double> diff{};

        t_beg = std::chrono::system_clock::now();

        int correct = 0;
        long long int total = 0;
        for (int i = 0; i < lines.size(); i++) {
            auto ids = tokenizer(lines[i]);
            if (ids == gts[i]) {
                correct += 1;
            }
            total += ids.size();
        }
        t_end = std::chrono::system_clock::now();
        diff = t_end - t_beg;
        auto elapsed_time = diff.count();
        std::cout << elapsed_time << " seconds" << std::endl;

        std::cout << lines.size() << std::endl;
        std::cout << static_cast<double>(correct) * 100.0 / lines.size() << " % Accuracy" << std::endl;
        std::cout << static_cast<double>(lines.size()) / elapsed_time << " RPS" << std::endl;
    }


}

int main() {
    std::ios::sync_with_stdio(false);

    std::forward_list<int> d;
    for(int i=0;i<5;i++){
        d.push_front(i);
    }

    for(auto&e:d){
        cout << e << endl;
    }
    exit(1);

    test(), exit(0);

    FlashBertTokenizer tokenizer("../config/vocab.txt", true);

    const std::string text = "이렇게 하지만 과학자 연구 결과 스피노 사우루스 4 4족 보행 밝였습니다 이렇게 잘 보셨나 ㅎㅎ 스피노 사우루스 이제 4 4족 보행인 것 ㅎㅎ 아직 증거 부족";
    std::vector<int> ids = tokenizer(text);
    for (auto &e: ids) {
        std::cout << e << ", ";
    }
    std::cout << std::endl;
    return 0;
}