// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "cell.hpp"

#include <cassert>

const Direction::Type Direction::all[] = { Direction::top, Direction::bottom, Direction::left, Direction::right };

Direction::Direction(Type d)
    : dir(d)
{
}

Direction Direction::rotate(bool clockwise) const
{
    switch (dir) {
        case top:
            return clockwise ? right : left;
        case bottom:
            return clockwise ? left : right;
        case left:
            return clockwise ? top : bottom;
        case right:
            return clockwise ? bottom : top;
    }
    assert(false && "invalid direction");
}

Direction Direction::opposite() const
{
    switch (dir) {
        case top:
            return bottom;
        case bottom:
            return top;
        case left:
            return right;
        case right:
            return left;
    }
    assert(false && "invalid direction");
}

Direction::operator Type() const
{
    return dir;
}

void Connection::rotate(bool clockwise)
{
    const Connection old = *this;
    reset();
    for (const auto it : Direction::all) {
        if (old.test(it)) {
            set(Direction(it).rotate(clockwise));
        }
    }
}

char Cell::save() const
{
    assert(0);
    return 0; // not implemented
}

bool Cell::load(char state)
{
    (void)state;
    assert(0);
    return false; // not implemented
}
