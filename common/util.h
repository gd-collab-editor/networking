#pragma once

#include <algorithm>
#include <vector>
#include <cstdint>

template <typename T>
inline void vec_remove_val(std::vector<T>& vec, T val) {
    vec.erase(std::remove(vec.begin(), vec.end(), val), vec.end());
}

typedef uint32_t block_t;
typedef uint8_t msg_len_t;
typedef std::vector<block_t> selection_list;

#define BITS_32 INTPTR_MAX != INT64_MAX