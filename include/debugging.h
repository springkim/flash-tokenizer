//
// Created by SPRING on 2025. 3. 12..
//

#ifndef DEBUGGING_H
#define DEBUGGING_H
#include<iostream>

template<template<typename... Args> class Container, typename T>
void print(Container<T> values) {
    for (auto &e: values) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void print(T value) {
    std::cout << value << std::endl;
}

static void separate_line(int n = 50) {
    std::string s;
    for (int i = 0; i < n; i++) {
        s.push_back('-');
    }
    std::cout << s << std::endl;
}

#endif //DEBUGGING_H
