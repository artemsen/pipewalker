// SPDX-License-Identifier: MIT
// Skin loader.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <SDL2/SDL.h>

#include <string>
#include <vector>

/** Skin loader. */
class Skin {
public:
    /**
     * Initialization.
     * @param name name of the skin loaded by default
     * @return image handle, nullptr on error
     */
    SDL_Surface* initialize(const std::string& name);

    /**
     * Load previous available skin.
     * @return image handle, nullptr on error
     */
    SDL_Surface* prev();

    /**
     * Load next available skin.
     * @return image handle, nullptr on error
     */
    SDL_Surface* next();

    std::string name; ///< Skin name

private:
    /**
     * Search skin files in the specified directory.
     * @param dir path to directory with skin files (png)
     */
    void search(const char* dir);

    /**
     * Load skin.
     * @param index index of the skin record
     * @return image handle, nullptr on error
     */
    SDL_Surface* load(size_t index);

    /**
     * Get skin name from file path.
     * @param path path to the skin file
     * @return skin name (file name without dir and extension)
     */
    std::string get_name(const std::string& path) const;

    std::vector<std::string> available; ///< List of available skins
    size_t current;                     ///< Index of the current skin
};
