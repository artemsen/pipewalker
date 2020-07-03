// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "level_ui.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "view.hpp"

#include <SDL2/SDL_opengles2.h>
//#include <cassert>

static LevelUI level;

ViewLevel::ViewLevel()
{
    const settings& set = settings::instance();
    level.create(set.level_id, set.level_width, set.level_height, set.level_wrap);
#ifdef NDEBUG
    level.reset();
#endif // NDEBUG
}

bool ViewLevel::draw()
{
    render::clear();

    ViewPort vp(level.width, level.height + LevelUI::vertical_pad);
    vp.apply();

    if (level.draw()) {
        sound::play(sound::clatz);
        if (level.completed) {
            sound::play(sound::complete);
        }
    }

    // title
    const float cell_size = level.cell_size();
    const float offset_y = cell_size * static_cast<float>(level.height) / 2 + ((1.0f - LevelUI::general_scale) / 2.0f);
    Painter({ -cell_size * 5.0f, offset_y + cell_size }, cell_size * 10.0f, cell_size).draw(Texture::title);

    // buttons
    Painter({ -cell_size * 5.0f, -offset_y }, cell_size, cell_size).draw(Texture::btn_reset);
    Painter({ cell_size * 4.0f, -offset_y }, cell_size, cell_size).draw(Texture::btn_sett);
    Painter({ -cell_size * 3.0f, -offset_y }, cell_size, cell_size).draw(Texture::btn_prev);
    Painter({ cell_size * 2.0f, -offset_y }, cell_size, cell_size).draw(Texture::btn_next);

    // level id
    Painter({ -cell_size * 1.7f, -offset_y }, cell_size * 3.0f, cell_size).draw(level.text_id());

    return level.animation();
}

bool ViewLevel::on_click(size_t x, size_t y, MouseButton btn)
{
    ViewPort vp(level.width, level.height + LevelUI::vertical_pad);
    const Coordinates world = vp.to_world(x, y);

    const Position pos = level.to_position(world);

    if (pos) {
        Cell& cell = level.get_cell(pos);
        if (cell.type != Cell::free) {
            if (btn == MouseButton::middle) {
                cell.locked = !cell.locked;
            } else if (!cell.locked) {
                level.rotate_pipe(pos, btn == MouseButton::left);
            }
        }
    }

    // printf("screen x:%lu y:%lu -> world x:%f y%f -> cell x:%lu y:%lu\n", x, y, world.x, world.y, pos.x, pos.y);

    return false;
}
