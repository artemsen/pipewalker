// SPDX-License-Identifier: MIT
// Fireworks used in level completion animation.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "firework.hpp"

#include "mtrand.hpp"

Firework::Firework(const SDL_Rect& init)
{
    initial = init;
    // center of the cell
    initial.x += initial.w / 2;
    initial.y += initial.h / 2;

    age_limit = 0;
    birth_time = 0;
}

void Firework::update()
{
    const uint64_t age = SDL_GetTicks64() - birth_time;
    if (age > age_limit) {
        // reinit
        current = initial;
        angle = 0;
        alpha = 1.0;

        birth_time = SDL_GetTicks64();
        age_limit = mtrand::get(1000, 3000);

        xstep = mtrand::get(initial.w, initial.w * 3);
        if (mtrand::get(0, 2)) {
            xstep = -xstep;
        }

        rotation = mtrand::get(90, 180);
        if (mtrand::get(0, 2)) {
            rotation = -rotation;
        }
    } else {
        // recalc
        const float phase = static_cast<float>(age) / age_limit;
        alpha = 1.0 - phase;
        angle = phase * rotation;
        current.x = initial.x + phase * xstep;
        current.y = initial.y - sin(M_PI * phase) * initial.w * 2;
    }
}
