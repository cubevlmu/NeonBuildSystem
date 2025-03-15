#pragma once

#include <string>

namespace nbs {

    void replaceAll(std::string& str, const char* from, const char* to);
    void fastConcat(const char*, const char*, char*);
    void fastConcat(const char*, char*);
    void fastInt2Str(int num, char* buffer);
}