// SPDX-License-Identifier: MIT
// Fireworks used in level completion animation.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "firework.hpp"

#include <cmath>

#include "mtrand.hpp"

Firework::Firework(const SDL_Rect& init)
    : initial(init)
    , birth_time(0)
{
}

void Firework::update()
{
    const uint64_t cur_time = SDL_GetTicks64();
    uint64_t age = cur_time - birth_time;
    if (birth_time == 0 || age > age_limit) {
        // reinitialize
        age = 0;
        birth_time = cur_time;
        age_limit = mtrand::get(500, 1500);
        variant = mtrand::get(0, 4);
        delta_x = static_cast<float>(mtrand::get(-1000, 1000)) / 1000;
    }
    // recalc state
    const float phase = static_cast<float>(age) / age_limit;
    alpha = 1.0 - phase;
    angle = phase * delta_x * 90;
    current.w = (initial.w / 2) * phase;
    current.h = (initial.h / 2) * phase;
    current.x = initial.x + (initial.w / 2 - current.w / 2);
    current.y = initial.y + (initial.h / 2 - current.h / 2);
    current.x += phase * delta_x * initial.w;
    current.y -= (initial.h / 1.2) * sin(M_PI * phase);
}
