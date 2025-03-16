#ifndef FLASHTOKENIZER_ENV_H
#define FLASHTOKENIZER_ENV_H

#include <string>
#include <sstream>

// Global variables for testing
int g_1 = 0;
int g_2 = 0;

std::string cpp_env() {
    std::stringstream ss;
    
    #ifdef __cplusplus
    ss << "C++ Version: " << __cplusplus << std::endl;
    #endif
    
    #ifdef __GNUC__
    ss << "GCC Version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
    #endif
    
    #ifdef _OPENMP
    ss << "OpenMP Version: " << _OPENMP << std::endl;
    #endif
    
    #ifdef __AVX2__
    ss << "AVX2: Supported" << std::endl;
    #else
    ss << "AVX2: Not supported" << std::endl;
    #endif
    
    #ifdef __AVX__
    ss << "AVX: Supported" << std::endl;
    #else
    ss << "AVX: Not supported" << std::endl;
    #endif
    
    #ifdef __SSE4_2__
    ss << "SSE4.2: Supported" << std::endl;
    #else
    ss << "SSE4.2: Not supported" << std::endl;
    #endif
    
    #ifdef __SSE4_1__
    ss << "SSE4.1: Supported" << std::endl;
    #else
    ss << "SSE4.1: Not supported" << std::endl;
    #endif
    
    #ifdef NDEBUG
    ss << "Build: Release" << std::endl;
    #else
    ss << "Build: Debug" << std::endl;
    #endif
    
    return ss.str();
}

#endif // FLASHTOKENIZER_ENV_H
