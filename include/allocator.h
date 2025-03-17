//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                                CustomAllocator
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
#ifndef QLJPAQPGSGDWPTKAFFJPJEEWSNXUZWXWPABAYXVXEMANBMJVLNBRCMMRDFQTCG
#define QLJPAQPGSGDWPTKAFFJPJEEWSNXUZWXWPABAYXVXEMANBMJVLNBRCMMRDFQTCG


//*********************************************************************************
// using STRING_LIST = std::list<std::string, FastPoolAllocator<std::string>>;

template<typename T>
struct FastPoolAllocator {
    using value_type = T;

    template<typename U>
    struct rebind {
        using other = FastPoolAllocator<U>;
    };

    struct Node {
        Node *next;
    };

    FastPoolAllocator() noexcept = default;

    template<typename U>
    explicit FastPoolAllocator(const FastPoolAllocator<U> &) noexcept {
    }

    static Node *freeList;

    T *allocate(std::size_t n) {
        if (n != 1)
            return static_cast<T *>(::operator new(n * sizeof(T)));
        if (freeList) {
            T *p = reinterpret_cast<T *>(freeList);
            freeList = freeList->next;
            return p;
        }
        constexpr std::size_t blockSize = 1024;
        T *block = static_cast<T *>(::operator new(blockSize * sizeof(T)));
        for (std::size_t i = 1; i < blockSize; ++i) {
            Node *node = reinterpret_cast<Node *>(block + i);
            node->next = freeList;
            freeList = node;
        }
        return block;
    }

    void deallocate(T *p, std::size_t n) noexcept {
        if (n != 1) {
            ::operator delete(p);
            return;
        }
        Node *node = reinterpret_cast<Node *>(p);
        node->next = freeList;
        freeList = node;
    }
};

template<typename T>
typename FastPoolAllocator<T>::Node *FastPoolAllocator<T>::freeList = nullptr;

template<typename T, typename U>
bool operator==(const FastPoolAllocator<T> &, const FastPoolAllocator<U> &) noexcept {
    return true;
}

template<typename T, typename U>
bool operator!=(const FastPoolAllocator<T> &, const FastPoolAllocator<U> &) noexcept {
    return false;
}
#endif
