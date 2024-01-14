// SPDX-License-Identifier: MIT
// UI renderer.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL.h>

#include <string>

/** UI renderer. */
class Render {
public:
    /** Available textures. */
    enum TextureId {
        WindowBkg,      // main environment background
        CellBkg,        // cell background
        Title,          // title ("PipeWalker")
        Lock,           // lock
        Firework0,      // fireworks particle
        Firework1,      // fireworks particle
        Firework2,      // fireworks particle
        Firework3,      // fireworks particle
        Sender,         // sender
        ReceiverOff,    // receiver (not connected)
        ReceiverOn,     // receiver (connected)
        PipeHalfOff,    // half of pipe (not connected)
        PipeHalfOn,     // half of pipe (connected)
        PipeHalfShadow, // half of pipe (shadow)
        PipeBentOff,    // bent pipe (not connected)
        PipeBentOn,     // bent pipe (connected)
        PipeBentShadow, // bent pipe (shadow)
        PipeStrOff,     // straight pipe (not connected)
        PipeStrOn,      // straight pipe (connected)
        PipeStrShadow,  // straight pipe (shadow)
        PipeForkOff,    // fork pipe (not connected)
        PipeForkOn,     // fork pipe (connected)
        PipeForkShadow, // fork pipe (shadow)
        ButtonPrev,     // button 'previous'
        ButtonNext,     // button 'next'
        ButtonOk,       // button 'OK'
        ButtonReset,    // button 'reset'
        ButtonCancel,   // button 'cancel'
        ButtonSettings, // button 'settings'
        ButtonCbOn,     // radio button (checkbox) 'On'
        ButtonCbOff,    // radio button (checkbox) 'Off'
        Font            // font texture
    };

    /**
     * Constructor.
     * @param renderer SDL renderer instance
     */
    Render(SDL_Renderer* renderer);

    /**
     * Load textures from image (skin).
     * @param image image to load
     * @return false if something went wrong
     */
    bool load(SDL_Surface* image);

    /** Clear render queue, must be called before drawing scene. */
    void clear();
    /** Flush render queue, must be called after drawing scene. */
    void flush();

    /**
     * Fill window background.
     * @param width,height size of the window
     */
    void fill_background(int width, int height);

    /**
     * Draw specified texture.
     * @param id texture type
     * @param dst position and size
     * @param angle rotation angle
     * @param alpha transparency control
     */
    void draw(TextureId id, SDL_Rect& dst, double angle = 0, double alpha = 1);

    /**
     * Draw text.
     * @param text text to draw
     * @param size font size in px
     * @param x,y top-left coordinates
     */
    void draw_text(const char* text, size_t size, int x, int y);

    /**
     * Get width of the text string in px.
     * @param text source text
     * @param size font size in px
     * @return string width in px
     */
    size_t text_width(const char* text, size_t size);

private:
    /** Texture description. */
    struct Texture {
        SDL_Rect rect;
        SDL_Texture* texture;
    };

    Texture textures[TextureId::Font + 1];
    size_t texunit_size;
    SDL_Renderer* render;
};
