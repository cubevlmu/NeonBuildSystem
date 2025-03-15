#pragma once

#include <cstddef>

namespace nbs {
#ifdef MAX_PATH
#undef MAX_PATH
#endif

    enum { MAX_PATH = 260 };

    using i8 = char;
    using u8 = unsigned char;
    using i16 = short;
    using u16 = unsigned short;
    using i32 = int;
    using u32 = unsigned int;

#ifdef _WIN32
    using i64 = long long;
    using u64 = unsigned long long;
#else	
    using i64 = long;
    using u64 = unsigned long;
#endif
    using uintptr = u64;
    using ptr = size_t;
}