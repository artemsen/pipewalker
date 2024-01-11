// SPDX-License-Identifier: MIT
// Window layout: describes the size/position of elements in a window.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "layout.hpp"

#include <algorithm>

constexpr size_t padding = 4;
constexpr size_t base_ratio = 8;

bool Layout::Button::own(int x, int y) const
{
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y &&
            y < rect.y + rect.h);
}

Layout::Button::operator SDL_Rect&()
{
    return rect;
}

SDL_Rect* Layout::Button::operator->()
{
    return &rect;
}

void Layout::resize(int width, int height)
{
    window.x = 0;
    window.y = 0;
    window.w = width;
    window.h = height;

    if (level_width && level_height) {
        update(level_width, level_height);
    }
}

void Layout::update(size_t width, size_t height)
{
    level_width = width;
    level_height = height;

    // margin: size of header and footer
    size_t margin = std::min(64ul, static_cast<size_t>(window.h) / 10);

    // puzzle field
    const int max_wnd_w = window.w - padding * 2;
    const int max_wnd_h = window.h - (margin * 2 + padding * 2);
    const float level_ratio =
        static_cast<float>(level_width) / static_cast<float>(level_height);
    if (max_wnd_w > static_cast<float>(max_wnd_h) * level_ratio) {
        field.h = max_wnd_h;
        field.w = static_cast<float>(field.h) * level_ratio;
    } else {
        field.w = max_wnd_w;
        field.h = static_cast<float>(field.w) / level_ratio;
    }
    field.x = window.w / 2 - field.w / 2;
    field.y = window.h / 2 - field.h / 2;

    // size of a single puzzle cell
    cell_size = static_cast<float>(field.w) / level_width;

    // base size
    base_size = std::min(field.w, field.h) / 10;

    // fix up margin size
    if (margin * base_ratio > static_cast<size_t>(field.w)) {
        margin = field.w / base_ratio;
    }

    // title
    title.h = margin;
    title.w = margin * base_ratio;
    title.x = window.w / 2 - title.w / 2;
    title.y = field.y - title.h - padding;

    // buttons in footer
    reset->x = field.x;
    reset->y = field.y + field.h + padding;
    reset->w = margin;
    reset->h = margin;
    settings->x = field.x + field.w - margin;
    settings->y = reset->y;
    settings->w = margin;
    settings->h = margin;
    lvlprev->x = field.x + margin + padding;
    lvlprev->y = reset->y;
    lvlprev->w = margin;
    lvlprev->h = margin;
    lvlnext->x = settings->x - margin - padding;
    lvlnext->y = reset->y;
    lvlnext->w = margin;
    lvlnext->h = margin;

    // level size buttons (settings specific)
    const int btn_x = field.x + base_size * 3.3;
    const int btn_y = field.y + base_size * 1.4;
    const int btn_s = base_size * 0.7;
    for (size_t i = 0; i < sizeof(lvlsize) / sizeof(lvlsize[0]); ++i) {
        Checkbox& cb = lvlsize[i];
        cb->x = btn_x;
        cb->y = btn_y + btn_s * i + padding * i;
        cb->w = btn_s;
        cb->h = btn_s;
    }

    // level mode buttons (settings specific)
    wrap->x = btn_x;
    wrap->y = lvlsize[3]->y + lvlsize[3]->h + base_size / 2;
    wrap->w = btn_s;
    wrap->h = btn_s;

    // sound control button (settings specific)
    sound->x = btn_x;
    sound->y = wrap->y + wrap->h + base_size / 2;
    sound->w = btn_s;
    sound->h = btn_s;

    // skin switch buttons (settings specific)
    skinprev->w = btn_s;
    skinprev->h = btn_s;
    skinprev->x = field.x + base_size;
    skinprev->y = field.y + field.h - base_size * 1.8;
    skinnext->w = btn_s;
    skinnext->h = btn_s;
    skinnext->x = field.x + field.w - btn_s - base_size;
    skinnext->y = skinprev.rect.y;
}
