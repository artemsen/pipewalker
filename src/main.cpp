// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "level.hpp"
#include "ui_console.hpp"
#include "ui_sdl.hpp"
#include "settings.hpp"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <getopt.h>

/**
 * @brief Print help usage info.
 *
 * @param[in] app application's file name
 */
static void print_help(const char* app)
{
    puts("PipeWalker logic game.");
    printf("Usage: %s [OPTION...]\n", app);
    puts("  -l, --level=NUM        Load speicified level, 0 for random");
    puts("  -c, --col=WIDTH        Number of columns of custom level size");
    puts("  -r, --row=HEIGHT       Number of rows of custom level size");
    puts("  -w, --no-wrap          Do not use warp mode");
    puts("  -t, --console          Use text based UI");
    puts("  -f, --fullscreen       Fullscreen mode (GUI mode only)");
    puts("  -s, --no-sound         Disable sound (GUI mode only)");
    puts("  -v, --version          Print version info and exit");
    puts("  -h, --help             Print this help and exit");
}

/**
 * @brief Print version info.
 */
static void print_version()
{
    puts("PipeWalker ver." VERSION ".");
    puts("Project page: https://github.com/artemsen/pipewalker");
}

/** @brief Application entry point. */
int main(int argc, char* argv[])
{
    settings& set = settings::instance();
    bool use_console = false;

    // clang-format off
    const struct option longOpts[] = {
        {"level",        required_argument, nullptr, 'l'},
        {"col",          required_argument, nullptr, 'c'},
        {"row",          required_argument, nullptr, 'r'},
        {"no-wrap",      no_argument,       nullptr, 'w'},
        {"console",      no_argument,       nullptr, 't'},
        {"fullscreen",   no_argument,       nullptr, 'f'},
        {"no-sound",     no_argument,       nullptr, 's'},
        {"version",      no_argument,       nullptr, 'v'},
        {"help",         no_argument,       nullptr, 'h'},
        {nullptr,        0,                 nullptr,  0 }
    };
    // clang-format on
    const char* shortOpts = "l:c:r:wtfsvh";

    opterr = 0; // prevent native error messages

    // parse arguments
    int val;
    while ((val = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1) {
        switch (val) {
            case 'l':
                set.level_id = strtoul(optarg, nullptr, 0);
                if (set.level_id != 0 && set.level_id > Level::max_id) {
                    fprintf(stderr, "Invalid level number: %s, max is %lu\n", optarg, Level::max_id);
                    return EXIT_FAILURE;
                }
                break;
            case 'c':
                set.level_width = strtoul(optarg, nullptr, 0);
                if (set.level_width < Level::min_size || set.level_width > Level::max_size) {
                    fprintf(stderr, "Invalid level width: %s, expected %lu<=N<=%lu\n", optarg, Level::min_size,
                            Level::max_size);
                    return EXIT_FAILURE;
                }
                break;
            case 'r':
                set.level_height = strtoul(optarg, nullptr, 0);
                if (set.level_height < Level::min_size || set.level_height > Level::max_size) {
                    fprintf(stderr, "Invalid level height: %s, expected %lu<=N<=%lu\n", optarg, Level::min_size,
                            Level::max_size);
                    return EXIT_FAILURE;
                }
                break;
            case 'w':
                set.level_wrap = false;
                break;
            case 't':
                use_console = true;
                break;
            case 'f':
                set.fullscreen = true;
                break;
            case 's':
                set.sound = false;
                break;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            case 'h':
                print_help(argv[0]);
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Invalid option: %s\n", argv[optind - 1]);
                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Unexpected argument: %s\n", argv[optind]);
        return EXIT_FAILURE;
    }

    if (set.level_id == 0) {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        set.level_id = ts.tv_nsec & 0x03ffffff; // to be less than Level::max_id (99999999)
    }

    if (use_console) {
        ui_console::start();
    } else {
        ui_sdl::start();
    }

    return EXIT_SUCCESS;
}
