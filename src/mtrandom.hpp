// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstdint>

namespace mtrandom {

/**
 * @brief Set new seed for random sequence (Mersenne Twister PRNG).
 *
 * @param[in] seed initial seed value
 */
void seed(uint32_t seed);

/**
 * @brief Get float random number in range [min_val..max_val).
 *
 * @param[in] min_val min value
 * @param[in] max_val max value
 *
 * @return random value
 */
float get(float min_val, float max_val);

/**
 * @brief Get integral random number in range [min_val..max_val).
 *
 * @param[in] min_val min value
 * @param[in] max_val max value
 *
 * @return random value
 */
uint32_t get(uint32_t min_val, uint32_t max_val);

}; // namespace mtrandom
