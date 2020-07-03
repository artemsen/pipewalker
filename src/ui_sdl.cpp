// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "render.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "ui_sdl.hpp"
#include "view.hpp"

#include <SDL2/SDL.h>
#include <exception>
#include <string>

#define PW_SCREEN_WIDTH 520  ///< Initial screen (main window) width
#define PW_SCREEN_HEIGHT 620 ///< Initial screen (main window) height

class SdlError : public std::exception {
public:
    SdlError(const char* msg)
    {
        err = msg;
        const char* sdl_err = SDL_GetError();
        if (sdl_err && *sdl_err) {
            err += ": ";
            err += sdl_err;
        }
    }

    // From std::exception
    const char* what() const noexcept override
    {
        return err.c_str();
    }

private:
    /** @brief Error description buffer. */
    std::string err;
};

class SdlWindow {
public:
    SdlWindow() { }

    ~SdlWindow()
    {
        if (timer) {
            SDL_RemoveTimer(timer);
        }
        if (gl_ctx) {
            SDL_GL_DeleteContext(gl_ctx);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }

    void init()
    {
        if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
            throw SdlError("SDL initialization failed");
        }

        // setup OpenGL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

        const settings& set = settings::instance();

        if (set.sound) {
            sound::initialize();
        }

        // create window and OpenGL context
        Uint32 wnd_flags = SDL_WINDOW_OPENGL;
        if (set.fullscreen) {
            wnd_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
            wnd_flags |= SDL_WINDOW_RESIZABLE;
        }
        window = SDL_CreateWindow(PACKAGE_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, PW_SCREEN_WIDTH,
                                  PW_SCREEN_HEIGHT, wnd_flags);
        if (!window) {
            throw SdlError("Unable to create window");
        }
        gl_ctx = SDL_GL_CreateContext(window);
        if (!gl_ctx) {
            throw SdlError("Unable to create GL context");
        }
    }

    void expose() const
    {
        SDL_Event expose_event;
        expose_event.type = SDL_WINDOWEVENT;
        expose_event.window.event = SDL_WINDOWEVENT_EXPOSED;
        SDL_PushEvent(&expose_event);
    }

    // Timer - about 60 fps
    static constexpr Uint32 redraw_timeot = 1000 / 60;

    static Uint32 timer_callback(Uint32 interval, void*)
    {
        SDL_Event expose_event;
        expose_event.type = SDL_WINDOWEVENT;
        expose_event.window.event = SDL_WINDOWEVENT_EXPOSED;
        SDL_PushEvent(&expose_event);
        return interval;
    }

    void run()
    {
        bool done = false;
        SDL_Event event;

        while (!done) {
            if (SDL_WaitEvent(&event) == 0) {
                throw SdlError("Event loop error");
            }
            switch (event.type) {
                // case SDL_MOUSEMOTION:
                // break;
                case SDL_MOUSEBUTTONDOWN: {
                    View::MouseButton btn = View::MouseButton::left;
                    if (event.button.button == SDL_BUTTON_MIDDLE) {
                        btn = View::MouseButton::middle;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        btn = View::MouseButton::right;
                    }
                    view_level.on_click(event.motion.x, event.motion.y, btn);
                    expose();
                } break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if (event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod & KMOD_ALT))
                        done = true; // Alt+F4 pressed
                    // else
                    //    done = game_instance.on_key_press(event.key.keysym.sym);
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_EXPOSED:
                            //static int t = 1;
                            //printf("draw %08x\n", ++t);
                            if (view_level.draw()) {
                                if (!timer) {
                                    timer = SDL_AddTimer(redraw_timeot, &timer_callback, NULL);
                                }
                            } else if (timer) {
                                SDL_RemoveTimer(timer);
                                timer = 0;
                            }
                            SDL_GL_SwapWindow(window);
                            break;
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            render::resize(event.window.data1, event.window.data2);
                            break;
                    }
                    break;
            }
        }
    }

private:
    SDL_TimerID timer = 0;
    SDL_Window* window = nullptr;
    SDL_GLContext gl_ctx = nullptr;
    ViewLevel view_level;
};

namespace ui_sdl {
void start()
{
    SdlWindow sdl;
    sdl.init();

    render::initialize();
    render::load_textures(GAME_DATA_DIR "/Simple.png");

    sdl.run();
}
} // namespace ui_sdl
