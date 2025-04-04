#pragma once

#include <chrono>
#include <ctime>

namespace Common {
    typedef int64_t Nanos;

    constexpr Nanos NANOS_TO_MICROS = 1000;
    constexpr Nanos MICROS_TO_MILLIS = 1000;
    constexpr Nanos MILLIS_TO_SEC = 1000;
    constexpr Nanos NANOS_TO_MILLIS = NANOS_TO_MICROS * MICROS_TO_MILLIS;
    constexpr Nanos NANOS_TO_SEC = NANOS_TO_MILLIS * MILLIS_TO_SEC;

    inline auto getCurrentNanos() noexcept {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    inline auto& getCurrentTimeStr(std::string* time_str) noexcept {
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        time_str->assign(std::ctime(&now));

        if (!time_str->empty()) {
            time_str->at(time_str->size() - 1) = '\0';
        }
        return *time_str;
    }
}