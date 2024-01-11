// SPDX-License-Identifier: MIT
// Skin loader.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "skin.hpp"

#include "buildcfg.h"

#include <SDL2/SDL_image.h>
#include <dirent.h>

SDL_Surface* Skin::initialize(const std::string& name)
{
    SDL_Surface* image = nullptr;

    search(APP_DATADIR);

    if (available.empty()) {
        // try portable variant
        char* app_dir = SDL_GetBasePath();
        if (app_dir) {
            std::string path = app_dir;
            path += "data";
            search(path.c_str());
            SDL_free(app_dir);
        }
    }

    // search for specified skin
    for (size_t i = 0; i < available.size(); ++i) {
        const std::string& path = available[i];
        if (name == get_name(path)) {
            image = load(i);
            break;
        }
    }
    // fallback: load first available
    for (size_t i = 0; !image && i < available.size(); ++i) {
        image = load(i);
    }

    return image;
}

SDL_Surface* Skin::prev()
{
    SDL_Surface* image = nullptr;

    for (ssize_t i = current - 1; !image && i >= 0; --i) {
        image = load(i);
    }
    for (ssize_t i = available.size() - 1;
         !image && i > static_cast<ssize_t>(current); --i) {
        image = load(i);
    }

    return image;
}

SDL_Surface* Skin::next()
{
    SDL_Surface* image = nullptr;

    for (size_t i = current + 1; !image && i < available.size(); ++i) {
        image = load(i);
    }
    for (size_t i = 0; !image && i < current; ++i) {
        image = load(i);
    }

    return image;
}

SDL_Surface* Skin::load(size_t index)
{
    const std::string& path = available[index];
    SDL_Surface* image = IMG_Load(path.c_str());
    if (image) {
        name = get_name(path);
        current = index;
    }
    return image;
}

void Skin::search(const char* path)
{
    const char* ext = ".png";
    const size_t ext_len = strlen(ext);

    DIR* dir = opendir(path);
    if (dir) {
        dirent* ent;
        while ((ent = readdir(dir))) {
            const size_t len = strlen(ent->d_name);
            if (len <= ext_len || strcmp(ent->d_name + len - ext_len, ext)) {
                continue; // not a skin file
            }
            std::string full_path = path;
            full_path += '/';
            full_path += ent->d_name;
            available.push_back(full_path);
        }
        closedir(dir);
    }
}

std::string Skin::get_name(const std::string& path) const
{
    const size_t dir_end = path.find_last_of("\\/") + 1;
    const size_t ext_start = path.rfind('.');
    return path.substr(dir_end, ext_start - dir_end);
}
