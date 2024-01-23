// SPDX-License-Identifier: MIT
// Game level.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "cell.hpp"

/** Game level. */
class Level {
public:
    /** Max level number. */
    static constexpr size_t max_id = 99999999;
    /** Minimum level size. */
    static constexpr size_t min_size = 10;
    /** Maximum level size. */
    static constexpr size_t max_size = 50;

    /** Generate new level. */
    void generate();

    /**
     * Load cell state.
     * @param dump serialized state of all cells
     * @return loading status
     */
    bool load(const std::string& dump);

    /**
     * Save cells state.
     * @return serialized state of all cells
     */
    std::string save() const;

    /** Update level status: trace through pipes, refresh status, etc. */
    void update();

    /** Reset state (randomly rotate pipes). */
    void reset();

    /**
     * Initiate pipe rotation.
     * @param pos cell position
     * @param clockwise rotate direction
     */
    void rotate(const Position& pos, bool clockwise);

    /** Get cell instance for specified position. */
    Cell& get_cell(const Position& pos);
    const Cell& get_cell(const Position& pos) const;

    uint32_t id;                     ///< Map Id
    size_t width;                    ///< Field width
    size_t height;                   ///< Field height
    bool wrap;                       ///< Wrap mode flag
    Position sender;                 ///< Sender coordinate (zero patient)
    std::vector<Cell> cells;         ///< Cells array
    std::vector<Position> recievers; ///< Receivers array

    struct State {
        bool level_complete;    ///< Level complete
        bool rotation_active;   ///< One or more pipes in rotation
        bool rotation_complete; ///< Pipe rotation complete
    } state;

private:
    using Path = std::vector<Side>;

    /** Add one more receiver to level. */
    void add_reciever();

    /**
     * Find path from specified position to the sender.
     * @param from position to start
     * @param visited set of visited cells
     * @param path array of directions
     * @return true if path found
     */
    bool find_path(const Position& from, std::set<Position>& vizited,
                   Path& path) const;

    /**
     * Apply path as pipes to the level map.
     * @param start position to start
     * @param path array of directions
     */
    void apply_path(const Position& start, const Path& path);

    /**
     * Trace pipes: sets 'active' status for connected cells.
     * @param pos position to start
     */
    void trace_state(const Position& pos);

    /**
     * Get position of neighbor cell.
     * @param from origin position
     * @param to neighbor's side
     * @return neighbor position, can be the same as "from"
     */
    Position neighbor(const Position& from, Side to) const;
};
