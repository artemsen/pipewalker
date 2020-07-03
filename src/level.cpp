// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "level.hpp"
#include "mtrandom.hpp"

#include <algorithm>
#include <cassert>
#include <set>

void Level::create(uint32_t id, size_t width_sz, size_t height_sz, bool use_wrap)
{
    width = width_sz;
    height = height_sz;
    wrap = use_wrap;

    cells.resize(width * height);

    mtrandom::seed(id);

    const size_t rcv_max = cells.size() / 5;
    const size_t rcv_min = rcv_max - rcv_max / 3;

    bool installed = false;
    while (!installed) {
        // reset cells state
        std::fill(cells.begin(), cells.end(), Cell {});

        // install sender (server)
        sender.x = mtrandom::get(1u, width - 1);
        sender.y = mtrandom::get(1u, height - 1);
        get_cell(sender).type = Cell::sender;

        // install receivers (clients)
        size_t rcv_num = 0;
        while (rcv_num < rcv_max) {
            // get free cells
            std::vector<Position> free_cells;
            for (size_t x = 0; x < width; ++x) {
                for (size_t y = 0; y < height; ++y) {
                    Cell& cell = get_cell({ x, y });
                    if (cell.con.none())
                        free_cells.push_back({ x, y });
                }
            }
            if (free_cells.empty()) {
                rcv_num = rcv_max;
                break; // no more free cells
            }

            // find path from receiver to sender
            size_t install_try = width;
            bool rcv_installed = false;
            while (!rcv_installed && --install_try) {
                const Position& rcv_pos = free_cells[mtrandom::get(0u, free_cells.size())];
                // Cell rcv_cell;
                // rcv_cell.type = Cell::receiver;
                route_t path { { rcv_pos, Connection() } };
                size_t deep = 0;
                rcv_installed = find_path(path, deep);
                if (rcv_installed) {
                    // apply path
                    get_cell(path.begin()->pos).type = Cell::receiver;
                    for (const auto& it : path) {
                        Cell& cell = get_cell(it.pos);
                        cell.con = it.con;
                        if (cell.type == Cell::free) {
                            cell.type = Cell::pipe;
                        }
                    }
                    ++rcv_num;
                }
            }
            if (!rcv_installed) {
                break;
            }
        }
        installed = rcv_num >= rcv_min;
    }
}

void Level::reset()
{
    for (auto& it : cells) {
        if (it.con.any() && !it.locked) {
            const bool clockwize = mtrandom::get(0u, 2u);
            size_t count = mtrandom::get(0u, 3u);
            while (count--) {
                it.con.rotate(clockwize);
            }
        }
    }
}

void Level::trace()
{
    // reset state and collect receivers
    std::vector<Cell*> receivers;
    for (auto& it : cells) {
        it.active = false;
        if (it.type == Cell::receiver) {
            receivers.push_back(&it);
        }
    }

    // trace from sender
    trace_path(sender);

    // check completion status
    completed = true;
    for (const auto& it : receivers) {
        if (!it->active) {
            completed = false;
            break;
        }
    }
}

void Level::trace_path(const Position& pos)
{
    Cell& cur_cell = get_cell(pos);
    assert(!cur_cell.active);
    cur_cell.active = true;

    for (Direction dir : Direction::all) {
        if (cur_cell.con.test(dir)) {
            const Position next_pos = neighbor(pos, dir);
            if (next_pos) {
                Cell& next_cell = get_cell(next_pos);
                if (!next_cell.active && next_cell.con.test(dir.opposite())) {
                    trace_path(next_pos);
                }
            }
        }
    }
}

Position Level::neighbor(const Position& pos, const Direction& dir) const
{
    Position next = pos;

    switch (dir) {
        case Direction::top:
            if (next.y) {
                --next.y;
            } else if (wrap) {
                next.y = height - 1;
            }
            break;
        case Direction::bottom:
            if (next.y < height - 1) {
                ++next.y;
            } else if (wrap) {
                next.y = 0;
            }
            break;
        case Direction::left:
            if (next.x) {
                --next.x;
            } else if (wrap) {
                next.x = width - 1;
            }
            break;
        case Direction::right:
            if (next.x < width - 1) {
                ++next.x;
            } else if (wrap) {
                next.x = 0;
            }
            break;
    }

    // position is not changed, invalidate it
    if (next.x == pos.x && next.y == pos.y) {
        next.x = SIZE_MAX;
        next.y = SIZE_MAX;
    }

    return next;
}

Cell& Level::get_cell(const Position& pos)
{
    assert(pos.x < width && pos.y < height && "outside the field?");
    return cells[pos.y * width + pos.x];
}

const Cell& Level::get_cell(const Position& pos) const
{
    assert(pos.x < width && pos.y < height && "outside the field?");
    return cells[pos.y * width + pos.x];
}

bool Level::find_path(route_t& path, size_t& deep) const
{
    const Position cur_pos = path.back().pos;
    const Connection cur_con = path.back().con;

    if (++deep > width * height) {
        return false; // way is too long
    }

    // define order of possible directions
    std::vector<Direction> dir_order;
    dir_order.reserve(Direction::max);
    std::set<Direction> dir_avail(Direction::all, Direction::all + Direction::max);
    if (deep > (width + height) / 2) {
        // optimal direction
        const ssize_t delta_x = sender.x - cur_pos.x;
        const ssize_t delta_y = sender.y - cur_pos.y;
        assert(delta_x || delta_y); // are we at the finish?
        dir_order.push_back(delta_x < 0 ? Direction::left : Direction::right);
        dir_order.push_back(delta_y < 0 ? Direction::top : Direction::bottom);
        if (std::abs(delta_x) < std::abs(delta_y)) {
            std::swap(dir_order[0], dir_order[1]);
        }
        for (const auto& it : dir_order) {
            dir_avail.erase(it);
        }
    }
    // shuffle remained directions
    while (!dir_avail.empty()) {
        const size_t idx = mtrandom::get(0u, dir_avail.size());
        auto side = dir_avail.begin();
        std::advance(side, idx);
        dir_order.push_back(*side);
        dir_avail.erase(side);
    }

    // go through possible directions
    for (const Direction& dir : dir_order) {
        const Position next_pos = neighbor(cur_pos, dir);
        if (!next_pos) {
            continue; // out of the field with disabled wrap mode
        }

        // already visited?
        auto pred = [next_pos](auto o) { return o.pos.x == next_pos.x && o.pos.y == next_pos.y; };
        if (std::find_if(path.begin(), path.end(), pred) != path.end()) {
            continue;
        }

        // restore previous connections and add new one
        path.back().con = cur_con;
        path.back().con.set(dir);

        // try to connect with the neighbor
        Cell next_cell = get_cell(next_pos);
        if (next_cell.type != Cell::receiver && next_cell.con.count() < 3) {
            if (next_cell.type == Cell::sender && next_cell.con.any()) {
                continue; // sender has already connected
            }
            next_cell.con.set(dir.opposite());
            path.emplace_back(Step { next_pos, next_cell.con });
            if (next_cell.type == Cell::sender) {
                // special case - sender without connection, we are the first
                return true;
            }
            if (next_cell.con.count() > 2) {
                // connected to existing route
                return true;
            }
            // go to next cell
            if (find_path(path, deep)) {
                return true;
            }
            // remove current step and try another direction in the next iteration
            path.pop_back();
        }
    }
    return false;
}
