// SPDX-License-Identifier: MIT
// Program entry point.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "buildcfg.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <getopt.h>

#include <cstdlib>

#include "game.hpp"

/** Run game. */
bool run(State& state)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    IMG_Init(IMG_INIT_PNG);

    // create window
    SDL_Window* window = SDL_CreateWindow("PipeWalker", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 480, 640,
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // create renderer
    SDL_Renderer* render =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!render) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    // initialize game
    Game game(window, render);
    if (!game.initialize(state)) {
        return false;
    }

    // main game loop
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type != SDL_QUIT) {
                game.handle_event(event);
            } else {
                quit = true;
                break;
            }
        }
        if (!quit) {
            game.update();
            game.draw();
            SDL_Delay(33); // ~30 fps
        }
    }

    game.save(state);

    // clean up
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return true;
}

/** Application entry point. */
int main(int argc, char* argv[])
{
    State state;
    state.load();

    // clang-format off
    const struct option long_opts[] = {
        { "id",       required_argument, nullptr, 'i' },
        { "width",    required_argument, nullptr, 'c' },
        { "height",   required_argument, nullptr, 'r' },
        { "no-wrap",  no_argument,       nullptr, 'w' },
        { "no-sound", no_argument,       nullptr, 's' },
        { "version",  no_argument,       nullptr, 'v' },
        { "help",     no_argument,       nullptr, 'h' },
        { nullptr, 0, nullptr, 0 }
    };
    const char* short_opts = "i:c:r:wsvh";
    // clang-format on

    opterr = 0; // prevent native error messages

    // parse arguments
    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) !=
           -1) {
        switch (opt) {
            case 'i':
                state.level_id = strtoul(optarg, nullptr, 0);
                if (state.level_id <= 0 || state.level_id > Level::max_id) {
                    fprintf(stderr, "Invalid level id: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'c':
                state.level_width = strtoul(optarg, nullptr, 0);
                if (state.level_width < Level::min_size ||
                    state.level_width > Level::max_size) {
                    fprintf(stderr, "Invalid level width: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'r':
                state.level_height = strtoul(optarg, nullptr, 0);
                if (state.level_height < Level::min_size ||
                    state.level_height > Level::max_size) {
                    fprintf(stderr, "Invalid level height: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'w':
                state.level_wrap = false;
                break;
            case 's':
                state.sound = false;
                break;
            case 'v':
                printf("PipeWalker game version " APP_VERSION ".\n");
                return EXIT_SUCCESS;
            case 'h':
                printf("PipeWalker game version " APP_VERSION ".\n");
                printf("Usage: %s [OPTION...]\n", argv[0]);
                printf("  -i, --id=ID          set level Id (1-%zu)\n",
                       Level::max_id);
                printf("  -c, --width=COLUMNS  set level width (%zu-%zu)\n",
                       Level::min_size, Level::max_size);
                printf("  -r, --height=ROWS    set level height (%zu-%zu)\n",
                       Level::min_size, Level::max_size);
                puts("  -w, --no-wrap        disable warp mode");
                puts("  -s, --no-sound       disable sound");
                puts("  -v, --version        print version info and exit");
                puts("  -h, --help           print this help and exit");
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Invalid argument: %s\n", argv[optind - 1]);
                return EXIT_FAILURE;
        }
    }
    if (optind < argc) {
        fprintf(stderr, "Unexpected argument: %s\n", argv[optind]);
        return EXIT_FAILURE;
    }

    const bool rc = run(state);
    if (rc) {
        state.save();
    }

    return rc ? EXIT_SUCCESS : EXIT_FAILURE;
}
