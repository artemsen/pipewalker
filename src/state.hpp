// SPDX-License-Identifier: MIT
// Game state serializer.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstdint>
#include <string>

/** Game state. */
struct State {
    /**
     * Load state from external storage.
     * @return true if state was loaded
     */
    bool load();

    /**
     * Save state to external storage.
     * @return true if state was saved
     */
    bool save() const;

    uint32_t level_id = 1;
    bool level_wrap = true;
    size_t level_width = 10;
    size_t level_height = 10;
    std::string level_pipes;
    bool sound = true;
    std::string skin = "Simple";
};
