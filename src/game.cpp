// SPDX-License-Identifier: MIT
// Main game class.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "game.hpp"

#include "buildcfg.h"

#include <cmath>

struct LevelSize {
    size_t size;
    const char* name;
};
static const LevelSize level_sizes[] = {
    { 10, "10 * 10" },
    { 15, "15 * 15" },
    { 20, "20 * 20" },
    { 30, "30 * 30" },
};

Game::Game(SDL_Window* wnd, SDL_Renderer* renderer)
    : window(wnd)
    , render(renderer)
    , puzzle_mode(true)
{
}

bool Game::initialize(const State& state)
{
    SDL_Surface* skin_image = skin.initialize(state.skin);
    if (!skin_image) {
        printf("Failed to load textures\n");
        return false;
    }
    render.load(skin_image);
    SDL_FreeSurface(skin_image);

    sound.initialize();
    sound.enable = state.sound;

    level.id = state.level_id;
    level.width = state.level_width;
    level.height = state.level_height;
    level.wrap = state.level_wrap;
    level.generate();

    int width = 480, height = 640;
    SDL_GetWindowSize(window, &width, &height);
    layout.resize(width, height);
    layout.update(level.width, level.height);

    if (level.load(state.level_pipes)) {
        level.update();
    } else {
        reset_level(true);
    }

    return true;
}

void Game::handle_event(const SDL_Event& event)
{
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    if (!puzzle_mode) {
                        puzzle_mode = true;
                    } else {
                        SDL_Event quit {};
                        quit.type = SDL_QUIT;
                        SDL_PushEvent(&quit);
                    }
                    break;
                case SDLK_s:
                    sound.enable = !sound.enable;
                    break;
                case SDLK_r:
                    if (puzzle_mode) {
                        reset_level(false);
                    }
                    break;
                case SDLK_n:
                    if (puzzle_mode && level.id < level.max_id) {
                        ++level.id;
                        reset_level(true);
                    }
                    break;
                case SDLK_p:
                    if (puzzle_mode && level.id > 1) {
                        --level.id;
                        reset_level(true);
                    }
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            on_mouse_click(event.motion.x, event.motion.y, event.button.button);
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                const SDL_WindowEvent& ev = event.window;
                layout.resize(ev.data1, ev.data2);
                if (!fireworks.empty()) {
                    // reinit fireworks with new coordinates
                    create_fireworks();
                }
            }
            break;
    }
}

void Game::update()
{
    level.update();

    if (level.state.level_complete) {
        if (fireworks.empty()) {
            sound.play(Sound::Complete);
            create_fireworks();
        }
        // update fireworks
        for (auto& it : fireworks) {
            it.update();
        }
    }

    if (!level.state.level_complete && level.state.rotation_complete) {
        sound.play(Sound::Clatz);
    }
}

void Game::draw()
{
    render.clear();
    render.fill_background(layout.window.w, layout.window.h);
    render.draw(Render::Title, layout.title);

    if (puzzle_mode) {
        draw_puzzle();
    } else {
        draw_settings();
    }

    render.flush();
}

void Game::save(State& state) const
{
    state.level_id = level.id;
    state.level_width = level.width;
    state.level_height = level.height;
    state.level_wrap = level.wrap;
    state.level_pipes = level.save();
    state.skin = skin.name;
    state.sound = sound.enable;
}

void Game::draw_puzzle()
{
    SDL_Rect dst = { 0, 0, static_cast<int>(layout.cell_size),
                     static_cast<int>(layout.cell_size) };

    // cells background
    for (size_t y = 0; y < level.height; ++y) {
        for (size_t x = 0; x < level.width; ++x) {
            dst.x = layout.field.x + x * layout.cell_size;
            dst.y = layout.field.y + y * layout.cell_size;
            render.draw(Render::CellBkg, dst);
        }
    }

    // pipes shadow
    const int shadow_shift = layout.cell_size / 20;
    const int lift_shift = layout.cell_size / 16;
    for (size_t y = 0; y < level.height; ++y) {
        for (size_t x = 0; x < level.width; ++x) {
            const Cell& cell = level.get_cell({ x, y });
            dst.x = layout.field.x + x * layout.cell_size + shadow_shift;
            dst.y = layout.field.y + y * layout.cell_size + shadow_shift;
            Render::TextureId tid;
            switch (cell.pipe) {
                case Pipe::Half:
                    tid = Render::PipeHalfShadow;
                    break;
                case Pipe::Straight:
                    tid = Render::PipeStrShadow;
                    break;
                case Pipe::Bent:
                    tid = Render::PipeBentShadow;
                    break;
                case Pipe::Fork:
                    tid = Render::PipeForkShadow;
                    break;
                default:
                    continue;
            }
            if (cell.rotation()) {
                const int shift = lift_shift * sin(M_PI * cell.phase());
                dst.x += shift;
                dst.y += shift;
            }
            render.draw(tid, dst, cell.angle(), 0.3);
        }
    }

    // pipes
    for (size_t y = 0; y < level.height; ++y) {
        for (size_t x = 0; x < level.width; ++x) {
            Render::TextureId tid;
            const Cell& cell = level.get_cell({ x, y });
            dst.x = layout.field.x + x * layout.cell_size;
            dst.y = layout.field.y + y * layout.cell_size;
            switch (cell.pipe) {
                case Pipe::Half:
                    tid =
                        cell.active ? Render::PipeHalfOn : Render::PipeHalfOff;
                    break;
                case Pipe::Straight:
                    tid = cell.active ? Render::PipeStrOn : Render::PipeStrOff;
                    break;
                case Pipe::Bent:
                    tid =
                        cell.active ? Render::PipeBentOn : Render::PipeBentOff;
                    break;
                case Pipe::Fork:
                    tid =
                        cell.active ? Render::PipeForkOn : Render::PipeForkOff;
                    break;
                default:
                    continue;
            }
            if (cell.rotation()) {
                const int shift = lift_shift * sin(M_PI * cell.phase());
                dst.x -= shift;
                dst.y -= shift;
            }
            render.draw(tid, dst, cell.angle());
        }
    }

    // cell objects
    for (size_t y = 0; y < level.height; ++y) {
        for (size_t x = 0; x < level.width; ++x) {
            const Cell& cell = level.get_cell({ x, y });
            dst.x = layout.field.x + x * layout.cell_size;
            dst.y = layout.field.y + y * layout.cell_size;
            switch (cell.object) {
                case Cell::Sender:
                    render.draw(Render::Sender, dst);
                    break;
                case Cell::Receiver:
                    if (cell.active) {
                        render.draw(Render::ReceiverOn, dst);
                    } else {
                        render.draw(Render::ReceiverOff, dst);
                    }
                    break;
                default:
                    break;
            }
            if (cell.locked) {
                render.draw(Render::Lock, dst);
            }
        }
    }

    // buttons
    render.draw(Render::ButtonReset, layout.reset);
    render.draw(Render::ButtonPrev, layout.lvlprev);
    render.draw(Render::ButtonNext, layout.lvlnext);
    render.draw(Render::ButtonSettings, layout.settings);

    // level Id
    char level_id[32];
    sprintf(level_id, "%08u", level.id);
    const size_t font_sz = layout.reset->h * 0.7;
    const size_t width = render.text_width(level_id, font_sz);
    render.draw_text(level_id, font_sz,
                     layout.window.w / 2 - width / 2 - font_sz / 10,
                     layout.field.y + layout.field.h + font_sz / 3);

    // draw particles
    for (auto& it : fireworks) {
        const Render::TextureId tid =
            static_cast<Render::TextureId>(it.variant + Render::Firework0);
        render.draw(tid, it.current, it.angle, it.alpha);
    }
}

void Game::draw_settings()
{
    size_t width;
    const size_t font_sz = layout.lvlsize[0]->h;

    // level size switch
    const char* lvlsize = "Level size:";
    width = render.text_width(lvlsize, font_sz);
    render.draw_text(lvlsize, font_sz, layout.window.w / 2 - width / 2,
                     layout.lvlsize[0]->y - font_sz * 1.2);
    for (size_t i = 0; i < sizeof(level_sizes) / sizeof(level_sizes[0]); ++i) {
        render.draw(layout.lvlsize[i].checked ? Render::ButtonCbOn
                                              : Render::ButtonCbOff,
                    layout.lvlsize[i]);
        render.draw_text(level_sizes[i].name, layout.lvlsize[i]->h,
                         layout.lvlsize[i]->x + layout.lvlsize[i]->w,
                         layout.lvlsize[i]->y);
    }

    // wrap mode switch
    render.draw(layout.wrap.checked ? Render::ButtonCbOn : Render::ButtonCbOff,
                layout.wrap);
    render.draw_text("Wrap mode", layout.wrap->h,
                     layout.wrap->x + layout.wrap->w, layout.wrap->y);

    // sound control
    render.draw(sound.enable ? Render::ButtonCbOn : Render::ButtonCbOff,
                layout.sound);
    render.draw_text("Sound", layout.sound->h,
                     layout.sound->x + layout.sound->w, layout.sound->y);

    // skin switch
    const char* skin_label = "Skin";
    render.draw_text(skin_label, font_sz,
                     layout.skinprev->x + layout.skinprev->w,
                     layout.skinprev->y);
    width = render.text_width(skin.name.c_str(), font_sz);
    render.draw_text(skin.name.c_str(), font_sz,
                     layout.window.w / 2 - width / 2,
                     layout.skinprev->y + layout.skinprev->h * 1.2);
    render.draw(Render::ButtonPrev, layout.skinprev);
    render.draw(Render::ButtonNext, layout.skinnext);

    render.draw(Render::ButtonOk, layout.settings);
}

void Game::on_mouse_click(int x, int y, int button)
{
    if (puzzle_mode) {
        on_mouse_click_puzzle(x, y, button);
    } else {
        on_mouse_click_settings(x, y, button);
    }
}

void Game::on_mouse_click_puzzle(int x, int y, int button)
{
    if (!level.state.level_complete && x >= layout.field.x &&
        x < layout.field.x + layout.field.w && y >= layout.field.y &&
        y < layout.field.y + layout.field.h) {
        Position pos;
        pos.x = static_cast<float>(x - layout.field.x) / layout.cell_size;
        pos.y = static_cast<float>(y - layout.field.y) / layout.cell_size;
        Cell& cell = level.get_cell(pos);

        if (!cell.locked &&
            (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_RIGHT)) {
            level.rotate(pos, button == SDL_BUTTON_RIGHT);
        } else if (button == SDL_BUTTON_MIDDLE && cell.pipe != Pipe::None) {
            cell.locked = !cell.locked;
        }
    } else if (layout.reset.own(x, y)) {
        reset_level(false);
    } else if (layout.lvlprev.own(x, y)) {
        if (level.id > 1) {
            --level.id;
            reset_level(true);
        }
    } else if (layout.lvlnext.own(x, y)) {
        if (level.id < level.max_id) {
            ++level.id;
            reset_level(true);
        }
    } else if (layout.settings.own(x, y)) {
        puzzle_mode = false;
        layout.wrap.checked = level.wrap;
        for (size_t i = 0; i < sizeof(level_sizes) / sizeof(level_sizes[0]);
             ++i) {
            layout.lvlsize[i].checked = (level.width == level_sizes[i].size &&
                                         level.height == level_sizes[i].size);
        }
    }
}

void Game::on_mouse_click_settings(int x, int y, int /*button*/)
{
    SDL_Surface* skin_image = nullptr;

    if (layout.settings.own(x, y)) {
        bool regen_level = false;
        if (layout.wrap.checked != level.wrap) {
            level.wrap = layout.wrap.checked;
            regen_level = true;
        }
        for (size_t i = 0; i < sizeof(level_sizes) / sizeof(level_sizes[0]);
             ++i) {
            if (layout.lvlsize[i].checked) {
                const size_t lvl_sz = level_sizes[i].size;
                if (level.width != lvl_sz || level.height != lvl_sz) {
                    level.width = lvl_sz;
                    level.height = lvl_sz;
                    regen_level = true;
                    break;
                }
            }
        }
        if (regen_level) {
            reset_level(true);
        }
        puzzle_mode = true;
    } else if (layout.wrap.own(x, y)) {
        layout.wrap.checked = !layout.wrap.checked;
    } else if (layout.sound.own(x, y)) {
        sound.enable = !sound.enable;
    } else if (layout.skinprev.own(x, y)) {
        skin_image = skin.prev();
    } else if (layout.skinnext.own(x, y)) {
        skin_image = skin.next();
    } else {
        // handle level size switch
        int index = -1;
        for (size_t i = 0; index < 0 &&
             i < sizeof(layout.lvlsize) / sizeof(layout.lvlsize[0]);
             ++i) {
            if (layout.lvlsize[i].own(x, y)) {
                index = i;
            }
        }
        if (index >= 0) {
            for (size_t i = 0;
                 i < sizeof(layout.lvlsize) / sizeof(layout.lvlsize[0]); ++i) {
                layout.lvlsize[i].checked = (static_cast<size_t>(index) == i);
            }
        }
    }

    if (skin_image) {
        render.load(skin_image);
        SDL_FreeSurface(skin_image);
    }
}

void Game::reset_level(bool regen)
{
    fireworks.clear();

    if (regen) {
        level.generate();
        layout.update(level.width, level.height);
    }

#ifdef DEBUG
    level.get_cell(level.sender).rotate(false);
#else
    level.reset();
#endif // DEBUG

    level.update();
}

void Game::create_fireworks()
{
    const size_t fw_per_rcv = 4;
    fireworks.clear();
    fireworks.reserve(level.recievers.size() * fw_per_rcv);
    for (size_t y = 0; y < level.height; ++y) {
        for (size_t x = 0; x < level.width; ++x) {
            const Cell& cell = level.get_cell({ x, y });
            if (cell.object == Cell::Receiver) {
                SDL_Rect rect;
                rect.x = layout.field.x + x * layout.cell_size;
                rect.y = layout.field.y + y * layout.cell_size;
                rect.w = layout.cell_size;
                rect.h = layout.cell_size;
                for (size_t i = 0; i < fw_per_rcv; ++i) {
                    fireworks.push_back(Firework(rect));
                }
            }
        }
    }
}
