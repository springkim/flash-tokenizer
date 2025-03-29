cmake_minimum_required(VERSION 3.15)

set(PROJECT_VERSION_STR 1.1.6)

project(flash_tokenizer VERSION ${PROJECT_VERSION_STR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#if (WIN32)
#    set(PYTHON_VERSIONS "Python312" "Python311" "Python310" "Python39" "Python38" "Python37")
#    foreach (VERSION ${PYTHON_VERSIONS})
#        set(PYTHON_PATH "$ENV{USERPROFILE}/AppData/Local/Programs/Python/${VERSION}/python.exe")
#        if (EXISTS "${PYTHON_PATH}")
#            set(Python3_EXECUTABLE "${PYTHON_PATH}")
#            set(Python_EXECUTABLE "${PYTHON_PATH}")
#            message(STATUS "Found Python at: ${Python3_EXECUTABLE}")
#            break()
#        endif ()
#    endforeach ()
#elseif (UNIX AND NOT APPLE)
#    set(Python3_EXECUTABLE "/usr/bin/python3")
#    set(Python_EXECUTABLE "/usr/bin/python3")
#elseif (APPLE)
#    set(Python3_EXECUTABLE "/usr/local/bin/python3")
#    set(Python_EXECUTABLE "/usr/local/bin/python3")
#endif ()

option(BUILD_EXECUTABLE "Build the executable" ON)
option(BUILD_PYTHON_MODULE "Build the Python module" ON)


#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
#    if (WIN32)
#        find_package(OpenMP)
#    endif ()
#endif ()
find_package(OpenMP)

#file(GLOB LIB_SRC "3rdparty/src/*.cpp")
set(SRC_FILES src/bert_tokenizer.cpp)


if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILE_OPTIONS -Ofast -funroll-loops -floop-nest-optimize
            -fstrict-overflow -fomit-frame-pointer
            -fipa-cp-clone -fno-rtti)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-deprecated-declarations)
    add_definitions(-DDO_FORCE_INLINE)
    set(COMPILE_OPTIONS -O3 -flto -funroll-loops -ffast-math
            -fomit-frame-pointer -fvectorize -fslp-vectorize -fno-finite-math-only
            -fstrict-aliasing -finline-functions)
elseif (MSVC)
    set(COMPILE_OPTIONS /O2 /W1 /GL /Ot /Ob3 /fp:fast /arch:AVX2 /Zc:__cplusplus /EHsc- /GR-)
endif ()


if (BUILD_EXECUTABLE)

    add_executable(flash_tokenizer main.cpp ${SRC_FILES})
    target_include_directories(flash_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    #target_include_directories(flash_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/include)
    #    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    #        if (WIN32)
    #            if (OpenMP_CXX_FOUND)
    #                target_link_libraries(flash_tokenizer PRIVATE OpenMP::OpenMP_CXX)
    #            endif ()
    #        endif ()
    #    endif ()

    if (OpenMP_CXX_FOUND)
        target_link_libraries(flash_tokenizer PRIVATE OpenMP::OpenMP_CXX)
    endif ()

    target_compile_options(flash_tokenizer PRIVATE ${COMPILE_OPTIONS})
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

    pybind11_add_module(_core python/src/bindings.cpp)
    target_link_libraries(_core PRIVATE bert_tokenizer)
    #    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    #        if (WIN32)
    #            if (OpenMP_CXX_FOUND)
    #                target_link_libraries(_code PRIVATE OpenMP::OpenMP_CXX)
    #            endif ()
    #        endif ()
    #    endif ()

    if (OpenMP_CXX_FOUND)
        target_link_libraries(_core PRIVATE OpenMP::OpenMP_CXX)
    endif ()

    target_compile_options(_core PRIVATE ${COMPILE_OPTIONS})
    target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

    install(TARGETS _core DESTINATION flash_tokenizer)
endif ()