// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include "cell.hpp"

#include <vector>

/** @brief Game level. */
class Level {
public:
    /** @brief Max level number. */
    static constexpr size_t max_id = 99999999;
    /** @brief Minimum level size. */
    static constexpr size_t min_size = 10;
    /** @brief Maximum level size. */
    static constexpr size_t max_size = 500;

    virtual ~Level() = default;

    /**
     * @brief Generate new level.
     *
     * @param[in] id level Id (seed number)
     * @param[in] width_sz field width
     * @param[in] height_sz field height
     * @param[in] wrap wrap mode flag
     */
    virtual void create(uint32_t id, size_t width_sz, size_t height_sz, bool use_wrap);

    // void prev();
    // void next();

    /** @brief Reset state (randomly rotate pipes). */
    virtual void reset();

    /**
     * @brief Trace through pipes starting from the sender position.
     *        Refreshes 'active' status for all cells.
     */
    void trace();

    /** @brief Get cell instance for specified position. */
    Cell& get_cell(const Position& pos);
    const Cell& get_cell(const Position& pos) const;

    bool completed; ///< Level complete status
    size_t width;   ///< Field width
    size_t height;  ///< Field height

protected:
    struct Step {
        Position pos;
        Connection con;
    };
    using route_t = std::vector<Step>;

    Position neighbor(const Position& pos, const Direction& dir) const;
    virtual void trace_path(const Position& pos);
    bool find_path(route_t& path, size_t& deep) const;

private:
    std::vector<Cell> cells; ///< Cells array
    bool wrap;               ///< Wrap mode flag
    Position sender;         ///< Sender coordinate (server)
};
