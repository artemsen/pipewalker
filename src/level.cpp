// SPDX-License-Identifier: MIT
// Game level.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "level.hpp"

#include "mtrand.hpp"

void Level::generate()
{
    mtrand::seed(id);

    // reset cells state
    cells.resize(width * height);
    std::fill(cells.begin(), cells.end(), Cell {});

    // install sender (server)
    sender.x = mtrand::get(1ul, width - 1);
    sender.y = mtrand::get(1ul, height - 1);
    get_cell(sender).object = Cell::Sender;

    const size_t max_recievers = cells.size() / 5;
    recievers.clear();
    recievers.reserve(max_recievers);
    for (size_t i = 0; i < max_recievers; ++i) {
        add_reciever();
    }
}

bool Level::load(const std::string& dump)
{
    if (dump.length() != width * height) {
        return false;
    }

    for (size_t i = 0; i < width * height; ++i) {
        const char c = dump[i] - 'A';
        const bool lock = c & (1 << 4);
        const size_t sides = c & 0xf;
        cells[i].pipe.sides = sides;
        cells[i].locked = lock;
    }

    return true;
}

std::string Level::save() const
{
    std::string dump;
    dump.reserve(width * height);

    for (const Cell& cell : cells) {
        const unsigned long long sides = cell.pipe.sides.to_ullong();
        const char lock = cell.locked ? (1 << 4) : 0;
        const char state = lock + sides;
        dump += 'A' + state;
    }

    return dump;
}

void Level::update()
{
    // update cells state
    state.rotation_complete = false;
    Position pos;
    for (pos.y = 0; pos.y < height; ++pos.y) {
        for (pos.x = 0; pos.x < width; ++pos.x) {
            Cell& cell = get_cell(pos);
            cell.active = (pos == sender);
            switch (cell.update()) {
                case Cell::RotationComplete:
                    state.rotation_complete = true;
                    break;
                case Cell::Unchanged:
                case Cell::RotationInProgress:
                    break;
            }
        }
    }

    // trace from sender
    if (get_cell(sender).rotate_start == 0) {
        trace_state(sender);
    }

    // check completion status
    state.level_complete = true;
    for (const auto& it : recievers) {
        if (!get_cell(it).active) {
            state.level_complete = false;
            break;
        }
    }
}

void Level::reset()
{
    for (auto& it : cells) {
        if (it.pipe != Pipe::None && !it.locked) {
            const bool clockwize = mtrand::get(0, 2);
            size_t count = mtrand::get(0, 3);
            while (count--) {
                it.rotate(clockwize);
            }
        }
    }
}

void Level::rotate(const Position& pos, bool clockwise)
{
    get_cell(pos).rotate(clockwise);
    update();
}

Cell& Level::get_cell(const Position& pos)
{
    return cells[pos.y * width + pos.x];
}

const Cell& Level::get_cell(const Position& pos) const
{
    return cells[pos.y * width + pos.x];
}

void Level::add_reciever()
{
    // get free cells
    std::vector<Position> free_cells;
    free_cells.reserve(width * height);
    Position free_cell;
    for (free_cell.x = 0; free_cell.x < width; ++free_cell.x) {
        for (free_cell.y = 0; free_cell.y < height; ++free_cell.y) {
            const Cell& cell = get_cell(free_cell);
            if (cell.object == Cell::Empty && cell.pipe == Pipe::None &&
                (std::abs(static_cast<ssize_t>(free_cell.x) -
                          static_cast<ssize_t>(sender.x)) > 1 ||
                 std::abs(static_cast<ssize_t>(free_cell.y) -
                          static_cast<ssize_t>(sender.y)) > 1)) {
                free_cells.push_back(free_cell);
            }
        }
    }
    if (free_cells.empty()) {
        return;
    }

    // get random position
    const size_t free_index = mtrand::get(0ul, free_cells.size());
    const Position& reciever = free_cells[free_index];

    // find path from receiver to sender
    Path path;
    std::set<Position> vizited;
    vizited.insert(reciever);
    if (!find_path(reciever, vizited, path)) {
        return;
    }
    // update level
    get_cell(reciever).object = Cell::Receiver;
    apply_path(reciever, path);
    recievers.push_back(reciever);
}

bool Level::find_path(const Position& from, std::set<Position>& vizited,
                      Path& path) const
{
    // define order of possible directions
    std::vector<Side> sides;
    sides.reserve(4);

    if (vizited.size() < std::min(width, height)) {
        // random directions
        sides.push_back(Side::Left);
        sides.push_back(Side::Right);
        sides.push_back(Side::Top);
        sides.push_back(Side::Bottom);
        for (size_t i = 0; i < 4; ++i) {
            const size_t index0 = mtrand::get(0ul, sides.size());
            const size_t index1 = mtrand::get(0ul, sides.size());
            std::iter_swap(sides.begin() + index0, sides.begin() + index1);
        }
    } else {
        // shortest path
        const ssize_t delta_x = sender.x - from.x;
        const ssize_t delta_y = sender.y - from.y;
        if (std::abs(delta_x) > std::abs(delta_y)) {
            sides.push_back(delta_x < 0 ? Side::Left : Side::Right);
            sides.push_back(delta_y < 0 ? Side::Top : Side::Bottom);
            sides.push_back(delta_y >= 0 ? Side::Top : Side::Bottom);
            sides.push_back(delta_x >= 0 ? Side::Left : Side::Right);
        } else {
            sides.push_back(delta_y < 0 ? Side::Top : Side::Bottom);
            sides.push_back(delta_x < 0 ? Side::Left : Side::Right);
            sides.push_back(delta_x >= 0 ? Side::Left : Side::Right);
            sides.push_back(delta_y >= 0 ? Side::Top : Side::Bottom);
        }
        // check for possible forks
        for (const Side side : sides) {
            const Position next_pos = neighbor(from, side);
            if (next_pos != from) {
                const Cell& next_cell = get_cell(next_pos);
                if (next_cell.object == Cell::Empty &&
                    next_cell.pipe != Pipe::None &&
                    next_cell.pipe != Pipe::Fork) {
                    path.push_back(side);
                    return true;
                }
            }
        }
    }

    // go through possible directions
    for (const Side& side : sides) {
        const Position next_pos = neighbor(from, side);
        if (vizited.find(next_pos) != vizited.end()) {
            continue; // already visited
        }

        vizited.insert(next_pos);
        path.push_back(side);

        // try to connect with the neighbor
        const Cell& next_cell = get_cell(next_pos);
        if (next_cell.object == Cell::Sender && next_cell.pipe == Pipe::None) {
            return true; // connected to sender
        }
        if (next_cell.object == Cell::Empty && next_cell.pipe != Pipe::Fork) {
            if (next_cell.pipe != Pipe::None) {
                return true; // fork: connect to existing pipe
            }
            if (find_path(next_pos, vizited, path)) {
                return true; // route found
            }
        }

        // remove current step and try another direction
        path.pop_back();
    }

    return false;
}

void Level::apply_path(const Position& start, const Level::Path& path)
{
    Position pos = start;
    for (const Side side : path) {
        get_cell(pos).pipe.set(side);
        pos = neighbor(pos, side);
        get_cell(pos).pipe.set(side.opposite());
    }
}

void Level::trace_state(const Position& pos)
{
    Cell& cell = get_cell(pos);
    const Side sides[] = { Side::Top, Side::Right, Side::Bottom, Side::Left };

    for (const Side side : sides) {
        if (cell.pipe.get(side)) {
            const Position next_pos = neighbor(pos, side);
            Cell& next_cell = get_cell(next_pos);
            if (next_cell.rotate_start == 0 && !next_cell.active &&
                next_cell.pipe.get(side.opposite())) {
                next_cell.active = true;
                trace_state(next_pos);
            }
        }
    }
}

Position Level::neighbor(const Position& from, Side to) const
{
    Position next = from;

    switch (to) {
        case Side::Top:
            if (next.y) {
                --next.y;
            } else if (wrap) {
                next.y = height - 1;
            }
            break;
        case Side::Bottom:
            if (next.y < height - 1) {
                ++next.y;
            } else if (wrap) {
                next.y = 0;
            }
            break;
        case Side::Left:
            if (next.x) {
                --next.x;
            } else if (wrap) {
                next.x = width - 1;
            }
            break;
        case Side::Right:
            if (next.x < width - 1) {
                ++next.x;
            } else if (wrap) {
                next.x = 0;
            }
            break;
    }

    return next;
}
