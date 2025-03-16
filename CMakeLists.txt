cmake_minimum_required(VERSION 3.22)
project(FlashTokenizer LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES 70 75 80 86)

find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})

add_executable(FlashTokenizer main.cpp
        src/bert_tokenizer.cpp
        src/cuda_tokenizer.cpp
        src/cuda_tokenizer_kernel.cu)

target_include_directories(FlashTokenizer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(FlashTokenizer ${CUDA_LIBRARIES})
