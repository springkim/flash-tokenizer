cmake_minimum_required(VERSION 3.30)
project(FlashTokenizer)

set(CMAKE_CXX_STANDARD 17)


add_executable(FlashTokenizer main.cpp
        src/bert_tokenizer.cpp)

target_include_directories(FlashTokenizer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)