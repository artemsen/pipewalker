// SPDX-License-Identifier: MIT
// Single cell.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "cell.hpp"

#include <SDL2/SDL.h>

bool Position::operator==(const Position& other) const
{
    return y == other.y && x == other.x;
}

bool Position::operator!=(const Position& other) const
{
    return y != other.y || x != other.x;
}

Side::Side(Side::Type side)
{
    this->side = side;
}

Side Side::opposite() const
{
    switch (side) {
        case Top:
            return Bottom;
        case Bottom:
            return Top;
        case Left:
            return Right;
        case Right:
        default:
            return Left;
    }
}

Side::operator Type() const
{
    return side;
}

double Pipe::angle() const
{
    double angle = 0;

    switch (type) {
        case Half:
            if (sides[Side::Right]) {
                angle = 90;
            } else if (sides[Side::Bottom]) {
                angle = 180;
            } else if (sides[Side::Left]) {
                angle = 270;
            }
            break;
        case Straight:
            if (sides[Side::Right]) {
                angle = 90;
            }
            break;
        case Bent:
            if (sides[Side::Right] && sides[Side::Bottom]) {
                angle = 90;
            } else if (sides[Side::Bottom] && sides[Side::Left]) {
                angle = 180;
            } else if (sides[Side::Left] && sides[Side::Top]) {
                angle = 270;
            }
            break;
        case Fork:
            if (!sides[Side::Top]) {
                angle = 90;
            } else if (!sides[Side::Right]) {
                angle = 180;
            } else if (!sides[Side::Bottom]) {
                angle = 270;
            }
            break;
        case None:
            break;
    }

    return angle;
}

void Pipe::rotate(bool clockwise)
{
    auto old = sides;
    if (clockwise) {
        sides <<= 1;
        if (old[old.size() - 1]) {
            sides[0] = true;
        }
    } else {
        sides >>= 1;
        if (old[0]) {
            sides[sides.size() - 1] = true;
        }
    }
}

bool Pipe::get(const Side& side) const
{
    return sides[side];
}

void Pipe::set(const Side& side)
{
    sides[side] = true;

    switch (sides.count()) {
        case 1:
            type = Half;
            break;
        case 2:
            type = (sides[Side::Top] && sides[Side::Bottom]) ||
                    (sides[Side::Left] && sides[Side::Right])
                ? Straight
                : Bent;
            break;
        case 3:
            type = Fork;
            break;
        default:
            break;
    }
}

Pipe::operator Type() const
{
    return type;
}

Cell::Status Cell::update()
{
    if (rotate_start) {
        const Uint64 diff = SDL_GetTicks64() - rotate_start;
        if (diff > rotation_time) {
            // rotation completed
            rotate_start = 0;
            if (rotate_twice) {
                rotate_twice = false;
                rotate(rotate_clockwise);
            }
            return Cell::RotationComplete;
        }
        return Cell::RotationInProgress;
    }

    return Cell::Unchanged;
}

double Cell::angle() const
{
    double angle = pipe.angle();

    if (rotate_start) {
        const Uint64 diff = SDL_GetTicks64() - rotate_start;
        if (diff < rotation_time) {
            const float diff_angle = static_cast<float>(diff) /
                static_cast<float>(rotation_time) * 90.0f;
            angle = rotate_pipe.angle();
            if (rotate_clockwise) {
                angle += diff_angle;
            } else {
                angle -= diff_angle;
            }
        }
    }

    return angle;
}

void Cell::rotate(bool clockwise)
{
    if (rotate_start) {
        // rotation in progress
        if (rotate_clockwise == clockwise) {
            rotate_twice = true;
        } else if (rotate_twice) {
            rotate_twice = false;
        } else {
            // back rotation
            const Uint64 tick = SDL_GetTicks64();
            const Uint64 passed = tick - rotate_start;
            const Uint64 rest = rotation_time - passed;
            rotate_start = tick - rest;
            rotate_pipe = pipe;
            pipe.rotate(clockwise);
            rotate_clockwise = clockwise;
        }
    } else {
        rotate_pipe = pipe;
        rotate_clockwise = clockwise;
        rotate_start = SDL_GetTicks64();
        pipe.rotate(clockwise);
    }
}
