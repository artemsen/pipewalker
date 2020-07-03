// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL_opengles2.h>
#include <cstddef>

namespace render {
/**
 * @brief Initialize OpenGL.
 *
 * @throw std::runtime_error in case of errors
 */
void initialize();

/**
 * @brief Finalize OpenGL.
 */
void finalize();

/**
 * @brief Load textures from png file.
 *
 * @param[in] file path to the image file
 *
 * @throw std::runtime_error in case of errors
 */
void load_textures(const char* file);

/**
 * @brief Window resize handler.
 *
 * @param[in] width new window width
 * @param[in] height new window height
 */
void resize(size_t width, size_t height);

/**
 * @brief Clear the window and fill it with background image.
 */
void clear();
} // namespace render

/** @brief World coordinates.. */
struct Coordinates {
    float x;
    float y;
};

/** @brief Texture types. */
enum class Texture {
    bkg_env,      ///< Main environment background
    bkg_cell,     ///< Cell background
    title,        ///< Title ("PipeWalker")
    lock,         ///< Lock
    explosion,    ///< Explosion particle
    sender,       ///< Sender
    receiver_psv, ///< Passive receiver
    receiver_act, ///< Active receiver
    pipe_hlf_act, ///< Half of pipe (active, connected)
    pipe_hlf_psv, ///< Half of pipe (passive, not connected)
    pipe_bnt_act, ///< Bent pipe (active, connected)
    pipe_bnt_psv, ///< Bent pipe (passive, not connected)
    pipe_str_act, ///< Straight pipe (active, connected)
    pipe_str_psv, ///< Straight pipe (passive, not connected)
    pipe_frk_act, ///< Fork pipe (active, connected)
    pipe_frk_psv, ///< Fork pipe (passive, not connected)
    btn_prev,     ///< Button 'previous'
    btn_next,     ///< Button 'next'
    btn_ok,       ///< Button 'OK'
    btn_cancel,   ///< Button 'Cancel'
    btn_reset,    ///< Button 'reset'
    btn_sett,     ///< Button 'settings'
    btn_cb_on,    ///< Radio button (checkbox) 'On'
    btn_cb_off,   ///< Radio button (checkbox) 'Off'
    font          ///< Font texture
};

/** @brief Viewport. */
class ViewPort {
public:
    /**
     * @brief Constructor, loads current viewport for entire screen.
     */
    ViewPort();

    /**
     * @brief Constructor, creates squared viewport.
     *
     * @param[in] cols number of columns
     * @param[in] rows number of rows
     */
    ViewPort(size_t cols, size_t rows);

    ~ViewPort();

    /**
     * @brief Apply current viewport.
     */
    void apply();

    /**
     * @brief Convert screen to world coordinates.
     *
     * @param[in] x horizontal screen coordinate
     * @param[in] y vertical screen coordinate
     *
     * @return world coordinates
     */
    Coordinates to_world(size_t x, size_t y) const;

    /** @brief Current viewport. */
    GLint vp[4];
    /** @brief Backup viewport. */
    GLint bk[4] = { 0, 0, 0, 0 };
};

/** @brief Rectangle verticies, texture coordinates, their transformations and draw. */
struct Painter {
    static constexpr size_t coords = 4; // number of coordinates (4 corners)
    static constexpr size_t vc_num = 3; // number of 3D components (x,y,z)
    static constexpr size_t tc_num = 2; // number of 2D components (x,y)

    /** @brief Vertex indexes. */
    static const GLushort indices[6];

    /** @brief Rectangle vertices. */
    GLfloat vertices[coords * vc_num] = {
        // clang-format off
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        // clang-format on
    };

    /** @brief Texture coordinates. */
    GLfloat texture[coords * tc_num] = {
        // clang-format off
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
        // clang-format on
    };

    Painter() = default;

    /**
     * @brief Constructor: set absolute world coordinates for rectangle.
     *
     * @param[in] top_left top left corner coordinates
     * @param[in] bottom_right bottom right corner coordinates
     */
    Painter(const Coordinates& top_left, const Coordinates& bottom_right);
    Painter(const Coordinates& top_left, float width, float height);

    /**
     * @brief Set absolute world coordinates for rectangle.
     *
     * @param[in] top_left top left corner coordinates
     * @param[in] bottom_right bottom right corner coordinates
     *
     * @return self instance
     */
    //Painter& position(const Coordinates& top_left, const Coordinates& bottom_right);

    /**
     * @brief Set texture coordinates.
     *
     * @param[in] top_left top left corner coordinates
     * @param[in] bottom_right bottom right corner coordinates
     *
     * @return self instance
     */
    //Painter& texture(const Coordinates& top_left, const Coordinates& bottom_right);

    /**
     * @brief Relative moving.
     *
     * @param[in] x horizontal delta
     * @param[in] y vertical delta
     *
     * @return self instance
     */
    Painter& move(float x, float y);

    /**
     * @brief Clockwise rotation.
     *
     * @param[in] angle angle in degrees
     *
     * @return self instance
     */
    Painter& rotate(float angle);

    /**
     * @brief Set scale.
     *
     * @param[in] s scale factor (0..1)
     *
     * @return self instance
     */
    Painter& scale(float s);

    /**
     * @brief Set repeat counter for texture coordinates.
     *
     * @param[in] x number of texture wrap by horizontal
     * @param[in] y number of texture wrap by vertical
     *
     * @return self instance
     */
    Painter& repeat(float x, float y);

    /**
     * @brief Draw rectangle with texture.
     *
     * @param[in] tex texture type
     * @param[in] rect rectangle and texture coordinates
     */
    void draw(Texture tex) const;

    /**
     * @brief Draw text.
     *
     * @param[in] text text string (ASCII alpha, numeric and punctuations only)
     */
    void draw(const char* text);
};
