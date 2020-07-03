// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "mtrandom.hpp"

#include <cassert>

namespace mtrandom {

/** @brief Number of iterations for MT19937. */
static constexpr const uint32_t iter_num = 624;
/** @brief Middle word number. */
static constexpr const uint32_t middle = 397;
/** @brief State array. */
static uint32_t states[iter_num];
/** @brief Current state array index. */
static uint32_t state_index;

/** @brief Twiddle. */
constexpr uint32_t twiddle(uint32_t u, uint32_t v)
{
    return (((u & 0x80000000UL) | (v & 0x7fffffffUL)) >> 1) ^ ((v & 1UL) ? 0x9908b0dfUL : 0x0UL);
}

/**
 * @brief Regenerate state array.
 */
static void generate_state()
{
    for (uint32_t i = 0; i < iter_num - middle; ++i) {
        states[i] = states[i + middle] ^ twiddle(states[i], states[i + 1]);
    }
    for (uint32_t i = iter_num - middle; i < iter_num - 1; ++i) {
        states[i] = states[i + middle - iter_num] ^ twiddle(states[i], states[i + 1]);
    }
    states[iter_num - 1] = states[middle - 1] ^ twiddle(states[iter_num - 1], states[0]);
    state_index = 0;
}

/**
 * @brief Get random number.
 *
 * @return random number.
 */
static uint32_t random()
{
    if (state_index == iter_num) {
        generate_state(); // new state array is needed
    }
    uint32_t n = states[state_index++];
    n ^= (n >> 11);
    n ^= (n << 7) & 0x9d2c5680UL;
    n ^= (n << 15) & 0xefc60000UL;
    return (n ^ (n >> 18));
}

void seed(uint32_t seed)
{
    states[0] = seed;
    for (uint32_t i = 1; i < iter_num; ++i) {
        states[i] = 1812433253UL * (states[i - 1] ^ (states[i - 1] >> 30)) + i;
    }
    state_index = iter_num; // force regenerate state array
}

float get(float min_val, float max_val)
{
    const int32_t d = static_cast<int32_t>((max_val - min_val) * 1000.0f);
    const float n = min_val + static_cast<float>(random() % d) / 1000.0f;
    assert(n >= min_val && n <= max_val);
    return n;
}

uint32_t get(uint32_t min_val, uint32_t max_val)
{
    const uint32_t n = min_val + (random() % (max_val - min_val));
    assert(n >= min_val && n <= max_val);
    return n;
}

}; // namespace mtrandom
