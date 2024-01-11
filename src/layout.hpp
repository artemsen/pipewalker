// SPDX-License-Identifier: MIT
// Window layout: describes the size/position of elements in a window.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL.h>

/** Window layout. */
class Layout {
public:
    /** Button. */
    struct Button {
        /** Check if coordinates belong to the button. */
        bool own(int x, int y) const;

        operator SDL_Rect&();
        SDL_Rect* operator->();

        SDL_Rect rect;
    };

    /** Checkbox. */
    struct Checkbox : public Button {
        bool checked;
    };

    /**
     * Recalculate layout after window resize.
     * @param width,height new size of the window
     */
    void resize(int width, int height);

    /**
     * Recalculate layout with new level size.
     * @param width,height new level size
     */
    void update(size_t width, size_t height);

    size_t base_size; ///< Base size, depends on window size and aspect

    size_t level_width;  ///< Level width
    size_t level_height; ///< Level height
    float cell_size;     ///< Size of a single puzzle cell

    SDL_Rect window; ///< Window size
    SDL_Rect title;  ///< Title "PieWalker"
    SDL_Rect field;  ///< Puzzle field

    // Footer buttons
    Button reset;
    Button settings;
    Button lvlprev;
    Button lvlnext;

    // Settings specific
    Checkbox lvlsize[4]; ///< Level sizes
    Checkbox wrap;       ///< Wrap mode on/off
    Checkbox sound;      ///< Sound control
    Button skinprev;     ///< Load next skin
    Button skinnext;     ///< Load previous skin
};
