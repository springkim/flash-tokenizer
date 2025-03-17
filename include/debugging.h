#ifndef KZWRLWKYZZCSSRNDGYLRHQNKKMKVLWSJXWUPGGRQBZDYFQYMDYEELPLLEABYBA
#define KZWRLWKYZZCSSRNDGYLRHQNKKMKVLWSJXWUPGGRQBZDYFQYMDYEELPLLEABYBA
#include<iostream>

template<template<typename... Args> class Container, typename T>
void printd(Container<T> values) {
    for (auto &e: values) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void printd(T value) {
    std::cout << value << std::endl;
}

static void separate_line(int n = 50) {
    std::string s;
    for (int i = 0; i < n; i++) {
        s.push_back('-');
    }
    std::cout << s << std::endl;
}

static int g_1 = 0;
static int g_2 = 0;
#endif //DEBUGGING_H
