#pragma once

#include <chrono>

namespace nbs {

    struct Timer {
        typedef void(*end_callback)(long);
        Timer(end_callback cb) {
            m_cb = cb;
            m_start = std::chrono::high_resolution_clock::now();
        }
        ~Timer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
            m_cb(duration.count());
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> m_start;
        end_callback m_cb;
    };
}