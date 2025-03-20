#pragma once

#include <iostream>
#include <atomic>
#include <vector>

#include "macros.h"

namespace Common {
    // SPSC - Single Producer Single Consumer
    template<typename T>
    class LFQueue {        
        public:
            LFQueue() = delete;
            LFQueue(const LFQueue&) = delete;
            LFQueue(LFQueue&&) = delete;
            LFQueue& operator=(const LFQueue&) = delete;
            LFQueue& operator=(LFQueue&&) = delete;

            explicit LFQueue(size_t num_elements) : store_(num_elements, T()) {}

            auto getNextToWriteTo() noexcept {
                return &store_[next_write_index_];
            }

            auto updateWriteIndex() noexcept {
                next_write_index_ = (next_write_index_ + 1) % store_.size();
                num_elements_++;
            }

            auto getNextToRead() noexcept -> const T* {
                if (next_read_index_ == next_write_index_) {
                    return nullptr;
                }
                return &store_[next_read_index_];
            }

            auto updateReadIndex() noexcept {
                next_read_index_ = (next_read_index_ + 1) % store_.size();
                ASSERT(num_elements_ != 0, "Read an invalid element in " + std::to_string(pthread_self()));
                num_elements_--;
            }

            auto size() const noexcept {
                return num_elements_.load();
            }

        private:
            std::vector<T> store_;
            std::atomic<size_t> next_write_index_ = {0};
            std::atomic<size_t> next_read_index_ = {0};

            std::atomic<size_t> num_elements_ = {0};
    };
}
