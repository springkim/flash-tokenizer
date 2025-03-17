//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                               DS,CONFIG Macros
//
//               Copyright (C) 2025, Kim Bomm, all rights reserved.
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
#pragma once
#ifndef FTNACWLRUCDTKQHXMLSYPEJZLUZBRVCVUMSUUPTWEMUVFVKHXLBQEYZYDMCPWT
#define FTNACWLRUCDTKQHXMLSYPEJZLUZBRVCVUMSUUPTWEMUVFVKHXLBQEYZYDMCPWT
//#undef _GLIBCXX_HAVE_AT_QUICK_EXIT
// #undef _GLIBCXX_HAVE_QUICK_EXIT
// #include<stdlib.h>
#include "config.h"
#include <vector>
#include <list>
#include <deque>
#include <string>

#include <unordered_set>
#include "allocator.h"


#if defined(_WIN32) || defined(_WIN64)
#define SELECT_ANY  __declspec(selectany)
#elif defined(__linux__)
#define SELECT_ANY  __attribute__((weak))
#elif defined(__APPLE__)
#define SELECT_ANY  __attribute__((weak))
#endif

#if defined(_WIN32) || defined(_WIN64)
#if defined(__GNUC__)
#ifdef _OPENMP
#include "omp.h"
#else
#endif
#endif
#endif

#if LIST == 1
using STRING_LIST = std::list<std::string>;
using STRING_LIST_FAST = std::list<std::string, FastPoolAllocator<std::string> >;

using STRING_SEQUENCE = std::vector<std::string>;
#define CONCAT(A, B) (A).splice((A).end(),(B))
#elif DEQUE == 1
using STRING_LIST = std::deque<std::string>;
using STRING_LIST_FAST = std::deque<std::string, FastPoolAllocator<std::string> >;
#define CONCAT(A, B) std::move((B).begin(),(B).end(),std::back_inserter((A)))
#elif VECTOR == 1
using STRING_LIST = std::vector<std::string>;
#define CONCAT(A, B) std::move((B).begin(),(B).end(),std::back_inserter((A)))
#endif


#if LIST_IDS == 1
using INT_LIST = std::list<int>;
#define IDS_CONCAT(A, B) A.splice(A.end(),B)
#define INIT(A, B) ((void)0)
#define IDS_RETURN(A)   std::vector<int>((A).begin(),(A).end())
#elif DEQUE_IDS == 1
using INT_LIST = std::deque<INTEGER_TYPE>;
#define IDS_CONCAT(A, B) std::move((B).begin(),(B).end(),std::back_inserter((A)))
#define INIT(A,B) ((void)0)
#define IDS_RETURN(A)   std::vector<int>((A).begin(),(A).end())
#elif VECTOR_IDS == 1
using INT_LIST = std::vector<int>;
#define IDS_CONCAT(A, B) std::move((B).begin(),(B).end(),std::back_inserter((A)))
#define INIT(A, B) (A).reserve((B))
#define IDS_RETURN(A)   (A)
#endif


#if defined(_MSC_VER) || defined(__clang__)
#define EXECUTION
#else
#include <execution>
#define EXECUTION std::execution::par
#endif


#endif
