#pragma once

#include <string>
#include <fstream>
#include <cstdio>

#include "macros.h"
#include "time_utils.h"
#include "lock_free_queue.h"
#include "thread_pool.h"
#include "types.h"

namespace Common {
    enum class LogType : int8_t {
        CHAR = 0,
        INTEGER = 1,
        LONG_INTEGER = 2,
        LONG_LONG_INTEGER = 3,
        UNSIGNED_INTEGER = 4,
        UNSIGNED_LONG_INTEGER = 5,
        UNSIGNED_LONG_LONG_INTEGER = 6,
        FLOAT = 7,
        DOUBLE = 8
    };

    struct LogElement {
        LogType type_ = LogType::CHAR;
        union {
            char c;
            int i;
            long l;
            long ll;
            unsigned u;
            long unsigned lu;
            unsigned long long ull;
            float f;
            double d;
        } u_;
    };


    class Logger final {
    public:
        explicit Logger(const std::string& file_name) : file_name_(file_name) {
            file_.open(file_name_, std::ios::out | std::ios::app);
            ASSERT(file_.is_open(), "Failed to open file: " + file_name_);
            logger_thread_ = createAndRunThread(-1, "Common/Logger", [this](){flushQueue();});
            ASSERT(logger_thread_ != nullptr, "Failed to start logger thread");
        }

        ~Logger() {
            std::cerr << "Flushing and closing Logger for " << file_name_ << std::endl;

            while (queue_.size()) {
                std::this_thread::sleep_for(1ms);
            }
            running_ = false;
            logger_thread_->join();
            file_.close();
        }

        Logger() = delete;
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        auto pushValue(const LogElement& element) noexcept {
            *(queue_.getNextToWriteTo()) = element;
            queue_.updateWriteIndex();
        }

        auto pushValue(const char value) noexcept {
            pushValue(LogType::CHAR, {.c = value});
        }

        auto pushValue(const char * value) noexcept {
            while (*value) {
                pushValue(*value);
                ++value;
            }
        }

        auto pushValue(const std::string & value) noexcept {
            pushValue(value);
        }

        auto pushValue(const int value) noexcept {
            pushValue(LogType::INTEGER, {.i = value});
        }

        auto pushValue(const long value) noexcept {
            pushValue(LogType::LONG_INTEGER, {.l = value});
        }

        auto pushValue(const long long value) noexcept {
            pushValue(LogType::LONG_LONG_INTEGER, {.ll = value});
        }

        auto pushValue(const unsigned value) noexcept {
            pushValue(LogType::UNSIGNED_INTEGER, {.u = value});
        }

        auto pushValue(const unsigned long value) noexcept {
            pushValue(LogType::UNSIGNED_LONG_INTEGER, {.lu = value});
        }

        auto pushValue(const unsigned long long value) noexcept {
            pushValue(LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull = value});
        }

        auto pushValue(const float value) noexcept {
            pushValue(LogType::FLOAT, {.f = value});
        }

        auto pushValue(const double value) noexcept {
            pushValue(LogType::DOUBLE, {.d = value});
        }    

        auto flushQueue(){
            while (running_) {
                for (auto next = queue_.getNextToRead(); queue_.size() && next; next = queue_.getNextToRead()){
                    switch (next->type_) {
                        case LogType::CHAR:
                            file_ << element.u_.c;
                            break;
                        case LogType::INTEGER:
                            file_ << element.u_.i;
                            break;
                        case LogType::LONG_INTEGER:
                            file_ << element.u_.l;
                            break;
                        case LogType::LONG_LONG_INTEGER:
                            file_ << element.u_.ll;
                            break;
                        case LogType::UNSIGNED_INTEGER:
                            file_ << element.u_.u;
                            break;
                        case LogType::UNSIGNED_LONG_INTEGER:
                            file_ << element.u_.lu;
                            break;
                        case LogType::UNSIGNED_LONG_LONG_INTEGER:
                            file_ << element.u_.ull;
                            break;
                        case LogType::FLOAT:
                            file_ << element.u_.f;
                            break;
                        case LogType::DOUBLE:
                            file_ << element.u_.d;
                            break;
                        
                        default:
                            ASSERT(false, "Invalid log type: " + std::to_string(static_cast<int>(element.type_)));
                    }
                    queue_.updateReadIndex();
                    next = queue_.getNextToRead();                
                }

                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1ms);
            }
        }


        template<typename T, typename ... A>
        auto log (const char *s, const T& value, const A& ... args) noexcept {
            while(*s) {
                if (*s == '%') {
                    if (UNLIKELY(*(s + 1) == '%')) {
                        ++s;
                    } else {
                        pushValue(value);
                        log(s + 1, args...);
                        return;
                    }
                }
                pushValue(*s++);
            }
            FATAL("Extra arguments provided to log()");
        }

        auto log(const char *s) noexcept {
            while(*s) {
                if (*s == '%') {
                    if (UNLIKELY(*(s + 1) == '%')) {
                        ++s;
                    } else {
                        FATAL("missing arguments to log()");
                    }
                }
            }
        }
    
    private:
        const std::string file_name_;
        std::ofstream file_;
        LFQueue<LogElement> queue_;
        std::atomic<bool> running_{true};
        std::thread * logger_thread_ = nullptr;
    };

}