#pragma once

#include "globals.hpp"
#include <string>

namespace nbs {

    /// Utils type for dealing text with placeholder
    struct NText 
    {
    public:
        NText(const std::string_view& txt);

        operator const std::string() {
            return m_result;
        }
        operator const char*() {
            return m_result.c_str();
        }
        const std::string get() {
            return m_result;
        }

    private:
        u32 m_idx;
        u64 m_max;
        std::string m_result;
    };
}