// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <string>

struct settings {
    private:
        settings() = default;
    public:
        static settings& instance();

    uint32_t level_id = 1;
    size_t level_width = 10;
    size_t level_height = 10;
    bool level_wrap = true;
    bool sound = true;
    std::string theme;
    bool fullscreen = false;
};
