cmake_minimum_required(VERSION 3.15)

set(PROJECT_VERSION_STR 0.9.6)

project(flash_tokenizer VERSION ${PROJECT_VERSION_STR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#if (DEFINED ENV{CUSTOM_PYTHON})
#    set(CUSTOM_PYTHON $ENV{CUSTOM_PYTHON})
#else ()
#    set(CUSTOM_PYTHON OFF)
#endif ()
#option(CUSTOM_PYTHON "Build with other python" ${CUSTOM_PYTHON})
##### Find Python
if (WIN32)
    set(PYTHON_VERSIONS "Python313" "Python312" "Python311" "Python310" "Python39" "Python38" "Python37")
    foreach (VERSION ${PYTHON_VERSIONS})
        set(PYTHON_PATH "$ENV{USERPROFILE}/AppData/Local/Programs/Python/${VERSION}/python.exe")
        if (EXISTS "${PYTHON_PATH}")
            set(Python3_EXECUTABLE "${PYTHON_PATH}")
            set(Python_EXECUTABLE "${PYTHON_PATH}")
            message(STATUS "Found Python at: ${Python3_EXECUTABLE}")
            break()
        endif ()
    endforeach ()
elseif (UNIX AND NOT APPLE)
    set(Python3_EXECUTABLE "/usr/bin/python3")
    set(Python_EXECUTABLE "/usr/bin/python3")
elseif (APPLE)
    set(Python3_EXECUTABLE "/usr/local/bin/python3")
    set(Python_EXECUTABLE "/usr/local/bin/python3")
endif ()

option(BUILD_EXECUTABLE "Build the executable" ON)
option(BUILD_PYTHON_MODULE "Build the Python module" ON)


#file(GLOB SRC_FILES "src/bert_tokenizer.cpp")

set(SRC_FILES src/bert_tokenizer.cpp)
##### OpenMP
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    find_package(OpenMP)
    set(COMPILE_OPTIONS -O3 -funroll-loops)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILE_OPTIONS -O3 -march=native -flto -funroll-loops -ffast-math)
elseif (MSVC)
    find_package(OpenMP)
    set(COMPILE_OPTIONS /O2)
endif ()


if (BUILD_PYTHON_MODULE)
    if (CUSTOM_PYTHON)
        find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    endif ()


    execute_process(
            COMMAND "${Python_EXECUTABLE}" -c "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE _pybind11_cmake_dir
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    list(APPEND CMAKE_PREFIX_PATH "${_pybind11_cmake_dir}")
    find_package(pybind11 REQUIRED)


    add_library(bert_tokenizer STATIC
            ${SRC_FILES}
    )
    target_include_directories(bert_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if (OpenMP_CXX_FOUND)
        target_link_libraries(bert_tokenizer PRIVATE OpenMP::OpenMP_CXX)
    endif ()

    pybind11_add_module(_core python/src/bindings.cpp)
    target_link_libraries(_core PRIVATE bert_tokenizer)
    target_compile_options(_core PRIVATE ${COMPILE_OPTIONS})
    target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

    install(TARGETS _core DESTINATION flash_tokenizer)
endif ()

if (BUILD_EXECUTABLE)

    add_executable(flash_tokenizer main.cpp ${SRC_FILES})
    target_include_directories(flash_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if (OpenMP_CXX_FOUND)
        target_link_libraries(flash_tokenizer PRIVATE OpenMP::OpenMP_CXX)
    endif ()

    target_compile_options(flash_tokenizer PRIVATE ${COMPILE_OPTIONS})
endif ()