// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include "level.hpp"
#include "render.hpp"

#include <map>

/** @brief Level UI. */
class LevelUI : public Level {
public:
    /** @brief Vertical pad, in cell count. */
    static constexpr size_t vertical_pad = 2;
    /** @brief General scale. */
    static constexpr float general_scale = 0.97;
    /** @brief Duration of 90 degree turn, ms. */
    static constexpr size_t rotate_duration = 100;

    // from Level
    void create(uint32_t id, size_t width_sz, size_t height_sz, bool use_wrap) override;
    void reset() override;

    /**
     * @brief Draw level field.
     *
     * @return true if level state has changed
     */
    bool draw();

    /**
     * @brief Rotate pipe.
     *
     * @param[in] pos cell position
     * @param[in] clockwise direction
     */
    void rotate_pipe(const Position& pos, bool clockwise);

    /**
     * @brief Convert world coordinates to cell position.
     *
     * @param[in] coords world coordinates
     *
     * @return cell position, can be invalid if coordinates out of the field
     */
    Position to_position(const Coordinates& world) const;

    /**
     * @brief Check if animation is in progress.
     *
     * @return true if animation is in progress
     */
    bool animation() const;

    /**
     * @brief Get single cell size.
     *
     * @return cell size
     */
    float cell_size() const;

    /**
     * @brief Get level Id as a text string.
     *
     * @return level Id as a text string
     */
    const char* text_id() const;

private:
    // from Level
    void trace_path(const Position& pos) override;

    /**
     * @brief Get top left coordinates of game field.
     *
     * @return top left coordinates
     */
    Coordinates top_left() const;

    /**
     * @brief Handle rotations.
     *
     * @return true if level state has changed
     */
    bool refresh_rotation();

    /**
     * @brief Get texture type for a pipe in the specified cell.
     *
     * @param[in] pos cell position
     *
     * @return texture Id
     */
    Texture pipe_texture(const Position& pos) const;

    /**
     * @brief Get pipe's angle for specified cell.
     *
     * @param[in] pos cell position
     *
     * @return pipe angle in degree
     */
    float pipe_angle(const Position& pos) const;

    /**
     * @brief Get pipe's angle for specified connection.
     *
     * @param[in] con connection sides
     *
     * @return pipe angle in degree
     */
    float pipe_angle(const Connection& con) const;

    /**
     * @brief Initiate rotation of a pipe.
     *
     * @param[in] pos cell position
     * @param[in] clockwise direction
     *
     * @return true if pipe is already roating and second turn was added
     */
    bool rotate(const Position& pos, bool clockwise);

    /** @brief Create particles for congratulations. */
    void start_congratulations();

private:
    /** @brief Pipe rotation. */
    struct Rotation {
        uint32_t start_time; ///< Rotation start time
        float start_angle;   ///< Initial angle in degrees
        bool clockwise;      ///< Rotation direction
        bool twice;          ///< Twice rotation flag (180 degrees)
    };
    std::map<Position, Rotation> rotations; ///< Rotation pipes

    /** @brief Particle used for congratulations. */
    struct Particle {
        float shift;          ///< Direction and step of horizontal step
        size_t life_time;     ///< Life time in ms (speed)
        uint32_t start_time;  ///< Particle's birthday
        Coordinates init_pos; ///< Initial position (center of a cell)
        float size;           ///< Particle size

        /** @brief Rebirth of particle. */
        void reinit();
        /** @brief Draw particle. */
        void draw();
    };
    std::vector<Particle> particles; ///< Particles

    char str_id[9]; ///< Level Id as a text string
};
