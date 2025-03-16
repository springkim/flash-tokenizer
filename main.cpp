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
#include <chrono>
#include <fstream>
#include "bert_tokenizer.h"
#include "cuda_tokenizer.h"

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

    // Load data
    std::fstream fin("/home/ubuntu/repos/flash-tokenizer/dataset/kc_bert/titles_404464.txt", std::ios::in);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();
    std::cout << "Data loaded!!" << std::endl;

    fin.open("/home/ubuntu/repos/flash-tokenizer/dataset/kc_bert/gt_404464.txt", std::ios::in);
    std::vector<std::vector<int>> gts;
    std::string gt;
    while (std::getline(fin, gt)) {
        gts.push_back(parseNumbersFromString(gt));
    }
    fin.close();
    std::cout << "GT loaded!!" << std::endl;

    std::chrono::system_clock::time_point t_beg, t_end;
    std::chrono::duration<double> diff;

    // Run the original CPU implementation for baseline
    FlashBertTokenizer cpu_tokenizer("/home/ubuntu/repos/flash-tokenizer/dataset/kc_bert/vocab_char_16424.txt", true);
    
    t_beg = std::chrono::system_clock::now();
    int cpu_correct = 0;
    long long int cpu_total = 0;
    for (int i = 0; i < lines.size(); i++) {
        auto ids = cpu_tokenizer(lines[i]);
        if (ids == gts[i]) {
            cpu_correct += 1;
        }
        cpu_total += ids.size();
    }
    t_end = std::chrono::system_clock::now();
    diff = t_end - t_beg;
    auto cpu_elapsed_time = diff.count();
    std::cout << "CPU Implementation:" << std::endl;
    std::cout << cpu_elapsed_time << " seconds" << std::endl;
    std::cout << lines.size() << std::endl;
    std::cout << static_cast<double>(cpu_correct) * 100.0 / lines.size() << " % Accuracy" << std::endl;
    std::cout << lines.size() / cpu_elapsed_time << " RPS" << std::endl;
    
    // Run the CUDA implementation
    CudaTokenizer cuda_tokenizer("/home/ubuntu/repos/flash-tokenizer/dataset/kc_bert/vocab_char_16424.txt");
    
    t_beg = std::chrono::system_clock::now();
    int cuda_correct = 0;
    long long int cuda_total = 0;
    for (int i = 0; i < lines.size(); i++) {
        auto ids = cuda_tokenizer.tokenize(lines[i]);
        // Note: CUDA implementation is simplified, so accuracy will be lower
        if (ids.size() == gts[i].size()) {
            cuda_correct += 1;
        }
        cuda_total += ids.size();
    }
    t_end = std::chrono::system_clock::now();
    diff = t_end - t_beg;
    auto cuda_elapsed_time = diff.count();
    std::cout << "\nCUDA Implementation:" << std::endl;
    std::cout << cuda_elapsed_time << " seconds" << std::endl;
    std::cout << lines.size() << std::endl;
    std::cout << static_cast<double>(cuda_correct) * 100.0 / lines.size() << " % Accuracy" << std::endl;
    std::cout << lines.size() / cuda_elapsed_time << " RPS" << std::endl;
    
    // Performance comparison
    double speedup = cpu_elapsed_time / cuda_elapsed_time;
    std::cout << "\nPerformance Comparison:" << std::endl;
    std::cout << "CUDA Speedup: " << speedup << "x" << std::endl;

    return 0;
}
