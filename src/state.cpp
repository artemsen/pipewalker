// SPDX-License-Identifier: MIT
// Game state serializer.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "state.hpp"

#include <SDL2/SDL.h>

#include <vector>

#include "level.hpp"

static const char* app_name = "pipewalker";
static const char* state_file = "pipewalker.ini";

static const char* key_id = "id";
static const char* key_width = "width";
static const char* key_height = "height";
static const char* key_wrap = "wrap";
static const char* key_pipes = "pipes";
static const char* key_skin = "skin";
static const char* key_sound = "sound";

/** INI file reader/writer. */
class IniFile {
public:
    struct KeyValue {
        std::string key;
        std::string value;
    };

    IniFile(bool readonly)
        : io(nullptr)
    {
        char* dir = SDL_GetPrefPath(nullptr, app_name);
        if (dir) {
            std::string path = dir;
            path += state_file;
            SDL_free(dir);

            io = SDL_RWFromFile(path.c_str(), readonly ? "rb" : "wb");
        }
    }

    ~IniFile()
    {
        if (io) {
            SDL_RWclose(io);
        }
    }

    /** Write key-value. */
    bool write(const char* name, const std::string& value)
    {
        return SDL_RWwrite(io, name, strlen(name), 1) &&
            SDL_RWwrite(io, "=", 1, 1) &&
            SDL_RWwrite(io, value.c_str(), 1, value.length()) &&
            SDL_RWwrite(io, "\n", 1, 1);
    }

    /** Read key-value array. */
    std::vector<KeyValue> read()
    {
        std::vector<KeyValue> settings;
        std::vector<char> buffer(Level::max_size * Level::max_size + 128, 0);

        const size_t rd = SDL_RWread(io, &buffer[0], 1, buffer.size());

        size_t pos = 0;
        while (pos < rd) {
            const char* id = &buffer[pos];
            while (buffer[pos] != '=') {
                if (++pos == rd) {
                    return settings;
                };
            }
            buffer[pos++] = 0;
            const char* value = &buffer[pos];
            while (buffer[pos] != '\n') {
                if (++pos == rd) {
                    return settings;
                };
            }
            buffer[pos++] = 0;
            settings.push_back({ id, value });
        }

        return settings;
    }

    SDL_RWops* io;
};

bool State::load()
{
    IniFile ini(true);
    if (!ini.io) {
        return false;
    }

    for (const auto& it : ini.read()) {
        if (it.key == key_id) {
            const int id = std::stoi(it.value);
            if (id > 0 && static_cast<size_t>(id) <= Level::max_id) {
                level_id = id;
            }
        } else if (it.key == key_width) {
            const int sz = std::stoi(it.value);
            if (sz > 0 && static_cast<size_t>(sz) <= Level::max_size) {
                level_width = sz;
            }
        } else if (it.key == key_height) {
            const int sz = std::stoi(it.value);
            if (sz > 0 && static_cast<size_t>(sz) <= Level::max_size) {
                level_height = sz;
            }
        } else if (it.key == key_wrap) {
            level_wrap = std::stoi(it.value);
        } else if (it.key == key_pipes) {
            level_pipes = it.value;
        } else if (it.key == key_sound) {
            sound = std::stoi(it.value);
        } else if (it.key == key_skin) {
            skin = it.value;
        }
    }

    return true;
}

bool State::save() const
{
    IniFile ini(false);
    if (!ini.io) {
        return false;
    }

    return ini.write(key_id, std::to_string(level_id)) &&
        ini.write(key_width, std::to_string(level_width)) &&
        ini.write(key_height, std::to_string(level_height)) &&
        ini.write(key_wrap, std::to_string(level_wrap)) &&
        ini.write(key_pipes, level_pipes) &&
        ini.write(key_sound, std::to_string(sound)) &&
        ini.write(key_skin, skin);
}
