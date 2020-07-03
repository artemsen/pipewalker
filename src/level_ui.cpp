// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "level_ui.hpp"
#include "mtrandom.hpp"

#include <SDL2/SDL.h>
#include <cassert>
#include <map>
#include <vector>

void LevelUI::create(uint32_t id, size_t width_sz, size_t height_sz, bool use_wrap)
{
    Level::create(id, width_sz, height_sz, use_wrap);
    snprintf(str_id, sizeof(str_id), "%08u", id);
}

void LevelUI::reset()
{
    rotations.clear(); // stop current rotations

    Position pos;
    for (pos.y = 0; pos.y < height; ++pos.y) {
        for (pos.x = 0; pos.x < width; ++pos.x) {
            const Cell& cell = get_cell(pos);
            if (cell.type != Cell::free && !cell.locked && mtrandom::get(0u, 10u) < 9) {
                const bool clockwise = mtrandom::get(0u, 2u);
                rotate(pos, clockwise);
                if (mtrandom::get(0u, 2u)) {
                    rotate(pos, clockwise);
                }
            }
        }
    }

    trace();
}

bool LevelUI::draw()
{
    const bool state_changed = refresh_rotation();
    if (state_changed && completed) {
        puts("sss");
        start_congratulations();
    }

    const float cs = cell_size();
    const Coordinates tl = top_left();

    // background
    Painter({ tl.x, tl.y }, cs * width, cs * height).repeat(width, height).draw(Texture::bkg_cell);

    Coordinates world; // cell's world coordinates
    Position pos;      // cell's position
    for (pos.y = 0; pos.y < height; ++pos.y) {
        world.y = tl.y - static_cast<float>(pos.y) * cs;
        for (pos.x = 0; pos.x < width; ++pos.x) {
            world.x = tl.x + static_cast<float>(pos.x) * cs;

            const Cell& cell = get_cell(pos);
            const Painter tile({ world.x, world.y }, cs, cs);

            // pipe
            if (cell.type != Cell::free) {
                const Texture tex = pipe_texture(pos);
                const float angle = pipe_angle(pos);
                Painter pipe = tile;
                pipe.rotate(angle).draw(tex);
            }

            // sender/receiver
            if (cell.type == Cell::sender || cell.type == Cell::receiver) {
                Texture tex;
                if (cell.type == Cell::sender) {
                    tex = Texture::sender;
                } else {
                    tex = cell.active ? Texture::receiver_act : Texture::receiver_psv;
                }
                tile.draw(tex);
            }

            // lock
            if (cell.locked) {
                tile.draw(Texture::lock);
            }
        }
    }

    for (auto& it : particles) {
        it.draw();
    }

    return state_changed;
}

void LevelUI::rotate_pipe(const Position& pos, bool clockwise)
{
    const bool existing = rotate(pos, clockwise);
    if (!existing) {
        trace();
    }
}

Position LevelUI::to_position(const Coordinates& world) const
{
    const float cs = cell_size();
    const Coordinates tl = top_left();

    Position pos;
    const float cell_x = world.x - tl.x;
    if (cell_x >= 0.0f && cell_x <= cs * width) {
        pos.x = cell_x / cs;
    }
    const float cell_y = world.y + tl.y;
    if (cell_y >= 0.0f && cell_y <= cs * height) {
        pos.y = cell_y / cs;
    }
    return pos;
}

bool LevelUI::animation() const
{
    return !rotations.empty() || !particles.empty();
}

void LevelUI::trace_path(const Position& pos)
{
    // prevent going through rotaing pipes
    const auto it = rotations.find(pos);
    if (it == rotations.end()) {
        Level::trace_path(pos);
    }
}

float LevelUI::cell_size() const
{
    return 2.0f / std::max(width, height + vertical_pad) * general_scale;
}

const char* LevelUI::text_id() const
{
    return str_id;
}

Coordinates LevelUI::top_left() const
{
    const float cs = cell_size();
    return Coordinates { -cs * static_cast<float>(width) / 2.0f, cs * static_cast<float>(height) / 2.0f };
}

bool LevelUI::refresh_rotation()
{
    std::vector<Position> remove;

    const Uint32 tick = SDL_GetTicks();
    for (auto& it : rotations) {
        Rotation& rt = it.second;
        const Uint32 passed = tick - rt.start_time;
        if (passed >= rotate_duration) {
            // end of animation
            if (!rt.twice) {
                // mark as removed
                remove.push_back(it.first);
            } else {
                // start second turn
                Cell& cell = get_cell(it.first);
                rt.start_angle = pipe_angle(cell.con);
                rt.start_time = tick;
                rt.twice = false;
                cell.con.rotate(rt.clockwise);
            }
        }
    }

    const bool state_changed = !remove.empty();

    if (state_changed) {
        for (const auto& it : remove) {
            rotations.erase(it);
        }
        trace();
    }

    return state_changed;
}

Texture LevelUI::pipe_texture(const Position& pos) const
{
    Texture tex;

    const Cell& cell = get_cell(pos);
    assert(cell.con.any());

    switch (cell.con.count()) {
        case 1:
            tex = cell.active ? Texture::pipe_hlf_act : Texture::pipe_hlf_psv;
            break;
        case 2:
            if (cell.con.test(Direction::top) == cell.con.test(Direction::bottom)) {
                tex = cell.active ? Texture::pipe_str_act : Texture::pipe_str_psv;
            } else {
                tex = cell.active ? Texture::pipe_bnt_act : Texture::pipe_bnt_psv;
            }
            break;
        case 3:
            tex = cell.active ? Texture::pipe_frk_act : Texture::pipe_frk_psv;
            break;
        default:
            // assert(false && "no pipes in the cell");
            // tex = Texture::bkg_cell;
            break;
    }

    return tex;
}

float LevelUI::pipe_angle(const Position& pos) const
{
    float angle = pipe_angle(get_cell(pos).con);

    const auto it = rotations.find(pos);
    if (it != rotations.end()) {
        // animation in progress
        const Rotation& rt = it->second;
        const Uint32 passed = SDL_GetTicks() - rt.start_time;
        if (passed < rotate_duration) {
            const float delta = (static_cast<float>(passed) / rotate_duration) * 90.0f;
            angle = rt.start_angle + (rt.clockwise ? delta : -delta);
        }
    }

    return angle;
}

float LevelUI::pipe_angle(const Connection& con) const
{
    size_t rnum = 0; // number of turns 90 degrees clockwise

    assert(con.any());

    switch (con.count()) {
        case 1:
            if (con.test(Direction::right)) {
                rnum = 1; // 90 degrees
            } else if (con.test(Direction::bottom)) {
                rnum = 2; // 180 degrees
            } else if (con.test(Direction::left)) {
                rnum = 3; // 270 degrees
            }
            break;
        case 2:
            if (con.test(Direction::top) == con.test(Direction::bottom)) {
                if (con.test(Direction::right)) {
                    rnum = 1; // 90 degrees
                }
            } else {
                if (con.test(Direction::top) && con.test(Direction::left)) {
                    rnum = 3; // 270 degrees
                } else if (con.test(Direction::bottom)) {
                    if (con.test(Direction::right)) {
                        rnum = 1; // 90 degrees
                    } else {
                        rnum = 2; // 180 degrees
                    }
                }
            }
            break;
        case 3:
            if (!con.test(Direction::top)) {
                rnum = 1; // 90 degrees
            } else if (!con.test(Direction::right)) {
                rnum = 2; // 180 degrees
            } else if (!con.test(Direction::bottom)) {
                rnum = 3; // 270 degrees
            }
            break;
    }

    return 90.0f * rnum;
}

bool LevelUI::rotate(const Position& pos, bool clockwise)
{
    assert(pos);

    auto it = rotations.find(pos);
    const bool found = it != rotations.end();

    if (!found) {
        // new cell to rotate
        Cell& cell = get_cell(pos);
        Rotation rt;
        rt.start_angle = pipe_angle(cell.con);
        rt.start_time = SDL_GetTicks();
        rt.clockwise = clockwise;
        rt.twice = false;
        rotations.insert(std::make_pair(pos, rt));
        cell.con.rotate(clockwise);
    } else {
        // rotation is already in progress
        Rotation& rt = it->second;
        if (clockwise == rt.clockwise) {
            // same direction
            rt.twice = true;
        } else {
            // reverse
            Cell& cell = get_cell(pos);
            rt.start_angle = pipe_angle(cell.con);
            rt.twice = false;
            rt.clockwise = clockwise;
            const Uint32 tick = SDL_GetTicks();
            const Uint32 passed = tick - rt.start_time;
            rt.start_time = tick;
            if (passed < rotate_duration) {
                rt.start_time -= rotate_duration - passed;
            }
            cell.con.rotate(clockwise);
        }
    }

    return found;
}

void LevelUI::start_congratulations()
{
    const float cs = cell_size();
    const Coordinates tl = top_left();

    Coordinates world; // cell's world coordinates
    Position pos;      // cell's position
    for (pos.y = 0; pos.y < height; ++pos.y) {
        world.y = tl.y - static_cast<float>(pos.y) * cs;
        for (pos.x = 0; pos.x < width; ++pos.x) {
            if (get_cell(pos).type == Cell::receiver) {
                world.x = tl.x + static_cast<float>(pos.x) * cs;
                Particle p;
                p.init_pos = world;
                p.size = cs;
                for (size_t i = 0; i < 3; ++i) {
                    p.reinit();
                    particles.push_back(p);
                }
            }
        }
    }
}

void LevelUI::Particle::reinit()
{
    shift = mtrandom::get(-0.5f, 0.5f);
    life_time = mtrandom::get(100U, 2000U);
    start_time = SDL_GetTicks();
}

void LevelUI::Particle::draw()
{
    const Uint32 passed = SDL_GetTicks() - start_time;
    if (passed > life_time) {
        reinit();
    } else {
        const float phase = static_cast<float>(passed) / life_time;
        const Coordinates pos { init_pos.x + size * phase * shift,
                                init_pos.y + 0.5f * size * static_cast<float>(sin(M_PI * phase)) };
        const float angle = 90.0f * shift * 2.0f * phase;
        Painter(pos, size, size).rotate(angle).scale(0.5f * phase).draw(Texture::explosion);
    }
}
