// SPDX-License-Identifier: MIT
// Main game class.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL.h>

#include <vector>

#include "firework.hpp"
#include "layout.hpp"
#include "level.hpp"
#include "render.hpp"
#include "skin.hpp"
#include "sound.hpp"
#include "state.hpp"

/** Main game class. */
class Game {
public:
    /**
     * Constructor.
     * @param window game window
     * @param renderer image renderer
     */
    Game(SDL_Window* wnd, SDL_Renderer* renderer);

    /**
     * Initialization.
     * @param state initial game state
     * @return false if something went wrong
     */
    bool initialize(const State& state);

    /**
     * Handle event.
     * @param event an SDL event to process
     */
    void handle_event(const SDL_Event& event);

    /**
     * Update game state.
     * @return true if animation is in progress
     */
    bool update();

    /** Draw scene. */
    void draw();

    /**
     * Save state.
     * @param state game state container
     */
    void save(State& state) const;

private:
    /** Draw puzzle view. */
    void draw_puzzle();

    /** Draw settings view. */
    void draw_settings();

    /**
     * Mouse click handler.
     * @param x,y mouse coordinates
     * @param button mouse button identifier
     */
    void on_mouse_click(int x, int y, int button);
    void on_mouse_click_puzzle(int x, int y, int button);
    void on_mouse_click_settings(int x, int y, int button);

    /**
     * Window resize handler.
     * @param regen regenerate level flag
     */
    void reset_level(bool regen);

    /** (Re)create fireworks particles. */
    void create_fireworks();

    SDL_Window* window; ///< Main window
    Layout layout;      ///< Window layout
    Sound sound;        ///< Sound support
    Level level;        ///< Game level
    Skin skin;          ///< Skin loader
    Render render;      ///< Image drawer
    bool puzzle_mode;   ///< Currently active mode (puzzle/settings)

    std::vector<Firework> fireworks; ///< Completion animation
};
