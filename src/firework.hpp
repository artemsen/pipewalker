// SPDX-License-Identifier: MIT
// Fireworks used in level completion animation.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

struct Firework {
    /**
     * Constructor.
     * @param init initial position (cell)
     */
    Firework(const SDL_Rect& init);

    /** Update firework state. */
    void update();

    SDL_Rect current; ///< Current position and size
    double angle;     ///< Current angle
    double alpha;     ///< Current transparency

private:
    SDL_Rect initial;  ///< Initial position and size
    size_t birth_time; ///< Creation timestamp
    size_t age_limit;  ///< Age limit in ms
    int xstep;         ///< Direction and horizontal step
    int rotation;      ///< Rotation direction and final angle
};
