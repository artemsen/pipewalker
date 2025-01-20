// SPDX-License-Identifier: MIT
// UI renderer.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "render.hpp"

#include <SDL2/SDL_image.h>

#include <cstring>
#include <memory>

using SdlSurface = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;

/** Skin image splitter. */
struct SkinImage {
    static constexpr char font_first_char = ' ';
    static constexpr char font_last_char = '~' + 1;
    static constexpr char tex_per_image = 16;

    SkinImage(SDL_Surface* img)
        : image(img)
        , unit_size(img->w / tex_per_image)
    {
    }

    const SDL_Rect* find(Render::TextureId id) const
    {
        struct Map {
            Render::TextureId id;
            SDL_Rect rect;
        };

        // clang-format off
        static const Map map[] = {
            { Render::WindowBkg,      { 0, 0, 2, 2 } },
            { Render::CellBkg,        { 2, 0, 2, 2 } },
            { Render::Title,          { 0, 6, 16, 2 } },
            { Render::Lock,           { 10, 0, 2, 2 } },
            { Render::Firework0,      { 12, 0, 1, 1 } },
            { Render::Firework1,      { 12, 1, 1, 1 } },
            { Render::Firework2,      { 13, 0, 1, 1 } },
            { Render::Firework3,      { 13, 1, 1, 1 } },
            { Render::Sender,         { 4, 0, 2, 2 } },
            { Render::ReceiverOff,    { 6, 0, 2, 2 } },
            { Render::ReceiverOn,     { 8, 0, 2, 2 } },
            { Render::PipeHalfOff,    { 0, 2, 2, 2 } },
            { Render::PipeHalfOn,     { 2, 2, 2, 2 } },
            { Render::PipeBentOff,    { 8, 2, 2, 2 } },
            { Render::PipeBentOn,     { 10, 2, 2, 2 } },
            { Render::PipeStrOff,     { 4, 2, 2, 2 } },
            { Render::PipeStrOn,      { 6, 2, 2, 2 } },
            { Render::PipeForkOff,    { 12, 2, 2, 2 } },
            { Render::PipeForkOn,     { 14, 2, 2, 2 } },
            { Render::ButtonPrev,     { 0, 4, 2, 2 } },
            { Render::ButtonNext,     { 2, 4, 2, 2 } },
            { Render::ButtonOk,       { 4, 4, 2, 2 } },
            { Render::ButtonCancel,   { 6, 4, 2, 2 } },
            { Render::ButtonReset,    { 8, 4, 2, 2 } },
            { Render::ButtonSettings, { 10, 4, 2, 2 } },
            { Render::ButtonCbOff,    { 12, 4, 2, 2 } },
            { Render::ButtonCbOn,     { 14, 4, 2, 2 } },
            { Render::Font,           { 0, 8, 16, 6 } },
        };
        // clang-format on

        for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); ++i) {
            if (map[i].id == id) {
                return &map[i].rect;
            }
        }
        return nullptr;
    }

    /* Get subimage from skin. */
    SDL_Surface* get(Render::TextureId id) const
    {
        const SDL_Rect* rect = find(id);
        if (!rect) {
            return nullptr;
        }

        SDL_Rect src = *rect;
        src.x *= unit_size;
        src.y *= unit_size;
        src.w *= unit_size;
        src.h *= unit_size;

        SDL_Rect dst;
        dst.x = 0;
        dst.y = 0;
        dst.w = src.w;
        dst.h = src.h;

        SdlSurface sub(SDL_CreateRGBSurface(0, dst.w, dst.h, 32, 0x000000ff,
                                            0x0000ff00, 0x00ff0000, 0xff000000),
                       &SDL_FreeSurface);
        if (sub && SDL_BlitSurface(image, &src, sub.get(), &dst)) {
            return nullptr;
        }
        return sub.release();
    }

    SDL_Surface* image;
    size_t unit_size;
};

Render::Render(SDL_Renderer* renderer)
    : render(renderer)
{
    memset(&textures, 0, sizeof(textures));
}

bool Render::load(SDL_Surface* image)
{
    SkinImage splitter(image);

    texunit_size = splitter.unit_size;

    for (size_t i = 0; i < sizeof(textures) / sizeof(textures[0]); ++i) {
        Texture& texture = textures[i];
        const bool is_shadow = (i == PipeHalfShadow || i == PipeBentShadow ||
                                i == PipeStrShadow || i == PipeForkShadow);

        const TextureId id = static_cast<TextureId>(i - (is_shadow ? 1 : 0));
        SdlSurface sub(splitter.get(id), &SDL_FreeSurface);
        if (!sub) {
            return false;
        }

        if (is_shadow) {
            // convert any color to black
            uint32_t* pixels = reinterpret_cast<uint32_t*>(sub->pixels);
            for (int y = 0; y < sub->h; ++y) {
                for (int x = 0; x < sub->w; ++x) {
                    uint32_t& color = *(pixels + y * sub->w + x);
                    if (color) {
                        color = 0xff000000;
                    }
                }
            }
        }

        // create texture from subimage
        SDL_Texture* tx = SDL_CreateTextureFromSurface(render, sub.get());
        if (!tx) {
            return false;
        }
        // put texture into array
        if (texture.texture) {
            SDL_DestroyTexture(texture.texture);
        }
        texture.texture = tx;
        texture.rect.w = sub->w;
        texture.rect.h = sub->h;
    }

    return true;
}

void Render::clear()
{
    SDL_RenderClear(render);
}

void Render::flush()
{
    SDL_RenderPresent(render);
}

void Render::fill_background(int width, int height)
{
    Texture& tex = textures[WindowBkg];
    SDL_Rect dst;
    dst.w = tex.rect.w;
    dst.h = tex.rect.h;
    for (dst.y = 0; dst.y < height; dst.y += dst.h) {
        for (dst.x = 0; dst.x < width; dst.x += dst.w) {
            SDL_RenderCopy(render, tex.texture, &tex.rect, &dst);
        }
    }
}

void Render::draw(TextureId id, SDL_Rect& dst, double angle, double alpha)
{
    Texture& tex = textures[id];
    SDL_SetTextureAlphaMod(tex.texture, alpha * 0xff);
    SDL_RenderCopyEx(render, tex.texture, &tex.rect, &dst, angle, nullptr,
                     SDL_FLIP_NONE);
}

void Render::draw_text(const char* text, size_t size, int x, int y)
{
    Texture& font = textures[Font];

    SDL_Rect src;
    src.w = texunit_size;
    src.h = texunit_size;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = size;
    dst.h = size;

    while (*text) {
        if (*text < SkinImage::font_first_char ||
            *text > SkinImage::font_last_char) {
            ++text;
            continue; // unprintable
        }

        // get character position inside the font texture
        const size_t index = *text - SkinImage::font_first_char;
        const size_t row = index % SkinImage::tex_per_image;
        const size_t col = index / SkinImage::tex_per_image;
        src.x = row * texunit_size;
        src.y = col * texunit_size;

        SDL_RenderCopy(render, font.texture, &src, &dst);

        dst.x += static_cast<float>(size) * 0.6;
        ++text;
    }
}

size_t Render::text_width(const char* text, size_t size)
{
    size_t width = 0;

    while (*text) {
        if (*text >= SkinImage::font_first_char &&
            *text <= SkinImage::font_last_char) {
            width += static_cast<float>(size) * 0.6;
        }
        ++text;
    }

    return width;
}
