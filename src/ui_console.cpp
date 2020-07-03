// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "level.hpp"
#include "settings.hpp"
#include "ui_console.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

class color {
public:
    // xterm color codes
    static constexpr int reset = 0;
    static constexpr int border = 90;     // border lines, bright black
    static constexpr int active = 92;     // active cell, bright green
    static constexpr int passive = 37;    // passive cell, white
    static constexpr int rcvpassive = 31; // passive receiver, red
    static constexpr int sender = 42;     // sender background, green
    static constexpr int grid = 40;       // grid background, black

    color(int clr)
    {
        control(clr);
    }

    ~color()
    {
        control(reset);
    }

    static void print(int clr, const char* text)
    {
        color c(clr);
        printf("%s", text);
    }

    static void control(int code)
    {
        printf("\033[%im", code);
    }
};

/**
 * @brief Get cell as a printable string.
 *
 * @param[in] cell cell to print
 *
 * @return cell view
 */
static const wchar_t* cell_view(const Cell& cell)
{
    const wchar_t* view = nullptr;

    switch (cell.con.count()) {
        case 1:
            assert(cell.type == Cell::sender || cell.type == Cell::receiver);
            if (cell.con.test(Direction::left)) {
                view = L"─═ ";
            } else if (cell.con.test(Direction::right)) {
                view = L" ═─";
            } else if (cell.con.test(Direction::top)) {
                view = L" ╧ ";
            } else if (cell.con.test(Direction::bottom)) {
                view = L" ╤ ";
            }
            break;
        case 2:
            if (cell.con.test(Direction::top) == cell.con.test(Direction::bottom)) {
                if (cell.con.test(Direction::top)) {
                    view = L" │ ";
                } else {
                    view = L"───";
                }
            } else {
                if (cell.con.test(Direction::top)) {
                    view = cell.con.test(Direction::left) ? L"─┘ " : L" └─";
                } else {
                    view = cell.con.test(Direction::left) ? L"─┐ " : L" ┌─";
                }
            }
            break;
        case 3:
            if (!cell.con.test(Direction::top)) {
                view = L"─┬─";
            } else if (!cell.con.test(Direction::bottom)) {
                view = L"─┴─";
            } else if (!cell.con.test(Direction::left)) {
                view = L" ├─";
            } else if (!cell.con.test(Direction::right)) {
                view = L"─┤ ";
            }
            break;
        default:
            view = L"   ";
            break;
    }

    assert(view);
    return view;
}

/**
 * @brief Print level field.
 *
 * @param[in] lvl level to print
 */
void print(const Level& lvl)
{
    // top border
    {
        color clr(color::border);
        printf("╔════");
        for (size_t x = 0; x < lvl.width; ++x) {
            printf("═══");
        }
        printf("╗\n");
    }

    // col numbers
    color::print(color::border, "║    ");
    for (size_t x = 1; x <= lvl.width; ++x) {
        printf("%2lu ", x);
    }
    color::print(color::border, "║\n");

    // separator between title and game field
    {
        color clr(color::border);
        printf("║   ╔");
        for (size_t x = 0; x < lvl.width; ++x) {
            printf("═══");
        }
        printf("╣\n");
    }

    // print field
    for (size_t y = 0; y < lvl.height; ++y) {
        for (size_t x = 0; x < lvl.width; ++x) {

            // left border with letter
            if (x == 0) {
                color::print(color::border, "║");
                printf(" %c ", 'a' + static_cast<char>(y));
                color::print(color::border, "║");
            }

            {
                const Cell& cell = lvl.get_cell({ x, y });

                // foreground colors
                int foreground = cell.active ? color::active : color::passive;
                if (cell.type == Cell::receiver && !cell.active) {
                    foreground = color::rcvpassive;
                }
                color clr(foreground);

                // background color
                int background = -1;
                if (cell.type == Cell::sender) {
                    background = color::sender;
                } else if (x % 2 != y % 2) {
                    background = color::grid;
                }
                if (background >= 0) {
                    color::control(background);
                }

                // print cell
                const wchar_t* view = cell_view(cell);
                printf("%S", view);
            }

            // right border
            if (x == lvl.width - 1) {
                color::print(color::border, "║");
            }
        }
        printf("\n");
    }

    // bottom border
    {
        color clr(color::border);
        printf("╚═══╩");
        for (size_t x = 0; x < lvl.width; ++x) {
            printf("═══");
        }
        printf("╝\n");
    }
}

static Position get_pos(const Level& lvl, const std::string& str)
{
    Position pos;
    if (str.size() > 1 && isalpha(str[0]) && isdigit(str[1])) {
        const size_t y = str[0] - 'a';
        const size_t x = strtoul(&str[1], nullptr, 0) - 1;
        if (y < lvl.height) {
            pos.y = y;
        }
        if (x < lvl.width) {
            pos.x = x;
        }
    }
    return pos;
}

namespace ui_console {

void start()
{
    setlocale(LC_ALL, "");

    const settings& set = settings::instance();

    Level lvl;

    while (true) {
        lvl.create(set.level_id, set.level_width, set.level_height, set.level_wrap);
#ifdef NDEBUG
        lvl.reset();
#endif // NDEBUG
        lvl.trace();

        Position last_pos;
        while (true) {
            printf("Level %u\n", set.level_id);
            print(lvl);

            printf("Enter cell coordinate to rotate the pipe (e.g a1): ");
            Position pos;
            std::string coord;
            std::getline(std::cin, coord);
            if (coord.empty()) {
                pos = last_pos;
            } else {
                pos = get_pos(lvl, coord);
                if (!pos) {
                    printf("Invalid coordinate\n");
                }
            }

            if (pos) {
                last_pos = pos;
                lvl.get_cell(pos).con.rotate(true);
                lvl.trace();
            }
        }
    }
}

} // namespace ui_console
