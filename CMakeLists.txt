cmake_minimum_required(VERSION 3.15)

set(PROJECT_VERSION_STR 0.8.0)

project(flash_tokenizer VERSION ${PROJECT_VERSION_STR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXECUTABLE "Build the executable" ON)
option(BUILD_PYTHON_MODULE "Build the Python module" OFF)


if (BUILD_PYTHON_MODULE)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

    execute_process(
            COMMAND "${Python_EXECUTABLE}" -c "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE _pybind11_cmake_dir
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    list(APPEND CMAKE_PREFIX_PATH "${_pybind11_cmake_dir}")
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
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(flash_tokenizer PRIVATE -O3)
    elseif (MSVC)
        target_compile_options(flash_tokenizer PRIVATE /O2)
    endif ()
endif ()