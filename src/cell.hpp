// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <bitset>

/** @brief Position (coordinates). */
struct Position {
    size_t x = SIZE_MAX;
    size_t y = SIZE_MAX;

    /** @brief Check if position is valid. */
    inline operator bool() const
    {
        return x != SIZE_MAX && y != SIZE_MAX;
    }

    /** @brief Compare coordinates. */
    inline bool operator<(const Position& other) const
    {
        return y < other.y || (y == other.y && x < other.x);
    }
};

/** @brief Four-way direction. */
class Direction {
public:
    enum Type { top, bottom, left, right };

    /** @brief Max number of sides (top, left, ...). */
    static constexpr size_t max = 4;
    /** @brief Set of all possible directions. */
    static const Direction::Type all[max];

    /**
     * @brief Constructor.
     *
     * @param[in] dir direction type
     */
    Direction(Type dir);

    /**
     * @brief Get direction after rotating on 90 degrees.
     *
     * @param[in] clockwise direction of rotation
     *
     * @return rotated direction
     */
    Direction rotate(bool clockwise) const;

    /**
     * @brief Get opposite direction.
     *
     * @return opposite direction
     */
    Direction opposite() const;

    operator Type() const;

private:
    /** @brief Direction type. */
    Type dir;
};

/** @brief Four-way connection. */
class Connection : public std::bitset<Direction::max> {
public:
    /**
     * @brief Rotate on 90 degrees.
     *
     * @param[in] clockwise direction of rotation
     */
    void rotate(bool clockwise);
};

/** @brief Single cell. */
struct Cell {
    /** @brief Cell content types. */
    enum Type { free, pipe, sender, receiver };

    /**
     * @brief Save cell state.
     *
     * @return serialized cell state as a printable character
     */
    char save() const;

    /**
     * @brief Load cell state.
     *
     * @param[in] state serialized cell state as a printable character
     *
     * @return false if specified state has invalid format
     */
    bool load(char state);

    Type type = free;    ///< Cell type
    Connection con;     ///< Connected sides
    bool active = false; ///< Cell connection state (active/passive)
    bool locked = false; ///< Cell lock status (locked/unlocked)
};
