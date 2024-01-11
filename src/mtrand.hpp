// SPDX-License-Identifier: MIT
// Mersenne Twister PRNG.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstdint>

namespace mtrand {

/**
 * Set new seed for random sequence.
 * @param seed initial seed value
 */
void seed(uint32_t seed);

/**
 * Get random 32bit number.
 * @return random number.
 */
uint32_t get();

/**
 * Get float random number in range [min_val..max_val).
 * @param min_val min value
 * @param max_val max value
 * @return random value
 */
template <typename T> T get(T min_val, T max_val)
{
    return min_val + (get() % (max_val - min_val));
}

}; // namespace mtrand
