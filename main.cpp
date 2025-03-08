#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include<chrono>
#include<fstream>
#include"bert_tokenizer.h"

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




int main() {
    std::ios::sync_with_stdio(false);

    FlashBertTokenizer tokenizer("../res/vocab_char_16424.txt", true);

    std::fstream fin("../res/titles_404464.txt", std::ios::in);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();
    std::cout << "Data loaded!!" << std::endl;

    fin.open("../res/gt_404464.txt", std::ios::in);
    std::vector<std::vector<int>> gts;
    std::string gt;
    while (std::getline(fin, gt)) {
        gts.push_back(parseNumbersFromString(gt));
    }
    fin.close();
    std::cout << "GT loaded!!" << std::endl;

    std::chrono::system_clock::time_point t_beg, t_end;
    std::chrono::duration<double> diff;

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
    std::cout << lines.size() / elapsed_time << " RPS" << std::endl;

    return 0;
}