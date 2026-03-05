#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "utils.h"

namespace bitonic::cpu
{

bool is_pow2(size_t x) { return x && ((x & (x - 1)) == 0); }

size_t next_pow2(size_t x)
{
    if (x <= 1)
        return 1;

    --x;

    for (size_t i = 1; i < sizeof(size_t) * 8; i <<= 1)
        x |= x >> i;

    return x + 1;
}

inline void round_up_vector(std::vector<int>& vec, bool incr_order)
{
    const int filler = incr_order ? std::numeric_limits<int>::max()
                                  : std::numeric_limits<int>::min();

    const std::size_t old_size = vec.size();
    const std::size_t new_size = is_pow2(old_size) ? old_size : next_pow2(old_size);

    vec.resize(new_size);
    std::fill(vec.begin() + old_size, vec.end(), filler);
}
} // namespace bitonic::cpu