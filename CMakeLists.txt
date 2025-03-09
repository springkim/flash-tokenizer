cmake_minimum_required(VERSION 3.15)

set(PROJECT_VERSION_STR 0.7.1)

project(flash_tokenizer VERSION ${PROJECT_VERSION_STR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXECUTABLE "Build the executable" OFF)
option(BUILD_PYTHON_MODULE "Build the Python module" ON)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    add_definitions(-DSSE2NEON_SUPPRESS_WARNINGS)
    message(STATUS "ARM64 architecture detected: SSE2NEON_SUPPRESS_WARNINGS macro defined")
endif()

if (BUILD_PYTHON_MODULE)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    find_package(pybind11 CONFIG REQUIRED)

    add_library(bert_tokenizer STATIC
            src/bert_tokenizer.cpp
    )
    target_include_directories(bert_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


    pybind11_add_module(_core python/src/bindings.cpp)
    target_link_libraries(_core PRIVATE bert_tokenizer)
    target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})


    install(TARGETS _core DESTINATION flash_tokenizer)
endif ()

if (BUILD_EXECUTABLE)

    add_executable(flash_tokenizer main.cpp src/bert_tokenizer.cpp)
    target_include_directories(flash_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif ()