#pragma once

#include <initializer_list>
#include <string>


#define FastConcat(NAME, ...) \
    size_t __fc__sz__ = ::nbs::calculateSize(__VA_ARGS__) + 1; \
    char NAME[__fc__sz__]; \
    ::nbs::fastConcatMulti(NAME, (size_t)__fc__sz__, {__VA_ARGS__})

namespace nbs {

#define Concat(NAME, A, B) char NAME[strlen(A)+strlen(B)]; fastConcat(NAME, A, B)

    void replaceAll(std::string& str, const char* from, const char* to);

    void fastConcat(char*, const char*, const char*);
    
    inline void fastConcatMulti(char* c, size_t max, std::initializer_list<const char*> parts) {
        size_t idx = 0;

        for (auto& part : parts) {
            if (idx + strlen(part) >= max) {
                return;
            }
            strcpy(c + idx, part);
            idx += strlen(part);
        }
        c[max - 1] = '\0';
    }

    inline void calculate(const char* sz, size_t& s) {
        s += strlen(sz);
    }
    template <typename... Args>
    inline size_t calculateSize(Args... args) {
        size_t realSize = 0;
        int _[] = { (calculate(args, realSize), 0) ... };
        (void)_;
        return realSize;
    }

    void fastInt2Str(int num, char* buffer);
}