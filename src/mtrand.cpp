// SPDX-License-Identifier: MIT
// Mersenne Twister PRNG.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "mtrand.hpp"

namespace mtrand {

/** Number of iterations for MT19937. */
static constexpr const uint32_t iter_num = 624;
/** Middle word number. */
static constexpr const uint32_t middle = 397;
/** State array. */
static uint32_t states[iter_num];
/** Current state array index. */
static uint32_t state_index;

/** Twiddle state. */
constexpr uint32_t twiddle(uint32_t u, uint32_t v)
{
    return (((u & 0x80000000UL) | (v & 0x7fffffffUL)) >> 1) ^
        ((v & 1UL) ? 0x9908b0dfUL : 0x0UL);
}

/** Regenerate state array. */
static void generate_state()
{
    for (uint32_t i = 0; i < iter_num - middle; ++i) {
        states[i] = states[i + middle] ^ twiddle(states[i], states[i + 1]);
    }
    for (uint32_t i = iter_num - middle; i < iter_num - 1; ++i) {
        states[i] =
            states[i + middle - iter_num] ^ twiddle(states[i], states[i + 1]);
    }
    states[iter_num - 1] =
        states[middle - 1] ^ twiddle(states[iter_num - 1], states[0]);
    state_index = 0;
}

void seed(uint32_t seed)
{
    states[0] = seed;
    for (uint32_t i = 1; i < iter_num; ++i) {
        states[i] = 1812433253UL * (states[i - 1] ^ (states[i - 1] >> 30)) + i;
    }
    state_index = iter_num; // force regenerate state array
}

uint32_t get()
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

}; // namespace mtrand
