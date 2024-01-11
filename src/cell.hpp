// SPDX-License-Identifier: MIT
// Single cell.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <bitset>

/** Position (coordinates). */
struct Position {
    size_t x;
    size_t y;
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    bool operator<(const Position& other) const
    {
        return y < other.y || (y == other.y && x < other.x);
    }
};

/** Four-way sides. */
class Side {
public:
    /** Sides. */
    enum Type { Top, Right, Bottom, Left };

    /** Max number of sides (top, left, ...). */
    static constexpr size_t max = 4;

    Side(Type side);
    operator Type() const;

    /**
     * Get opposite direction.
     * @return opposite direction
     */
    Side opposite() const;

private:
    Type side;
};

/** Game pipe. */
class Pipe {
public:
    /** Pipe type. */
    enum Type { None, Half, Bent, Straight, Fork };

    /**
     * Get pipe angle.
     * @return pipe angle
     */
    double angle() const;

    /**
     * Rotate pipe.
     * @param clockwise rotate direction
     */
    void rotate(bool clockwise);

    /**
     * Check if side connected.
     * @param side to check
     * @return true if side connected
     */
    bool get(const Side& side) const;

    /**
     * Add connected side.
     * @param side to connect
     */
    void set(const Side& side);

    operator Type() const;

    std::bitset<Side::max> sides; ///< Pipe sides
    Type type = None;             ///< Type of the pipe
};

/** Game cell. */
struct Cell {
    /** Total 90 degree pipe rotation time. */
    static constexpr size_t rotation_time = 300;

    /** Cell object type. */
    enum Object { Empty, Sender, Receiver };

    /** Cell state. */
    enum Status {
        Unchanged,         ///< Cell state unchanged
        RotationComplete,  ///< Rotation complete
        RotationInProgress ///< Updating in progress
    };

    /**
     * Update cell state.
     * @return cell's current status
     */
    Status update();

    /**
     * Get pipe angle.
     * @return pipe angle
     */
    double angle() const;

    /**
     * Rotate pipe.
     * @param clockwise rotate direction
     */
    void rotate(bool clockwise);

    Object object = Empty; ///< Installed object
    bool active = false;   ///< Cell connection state (active/passive)
    bool locked = false;   ///< Cell lock status (locked/unlocked)
    Pipe pipe;             ///< Pipe

    size_t rotate_start;   ///< Rotation start timestamp
    Pipe rotate_pipe;      ///< Rotation pipe
    bool rotate_twice;     ///< Twice rotation flag
    bool rotate_clockwise; ///< Rotate direction
};
