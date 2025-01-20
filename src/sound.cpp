// SPDX-License-Identifier: MIT
// Sound subsystem.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#include "sound.hpp"

#include "buildcfg.h"

#include <SDL2/SDL.h>

#include <cstring>
#include <string>

Sound::~Sound()
{
    for (size_t i = 0; i < sizeof(waves) / sizeof(waves[0]); ++i) {
        if (waves[i].data) {
            SDL_FreeWAV(waves[i].data);
        }
    }
}

bool Sound::initialize()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return false;
    }

    SDL_AudioSpec as;
    memset(&as, 0, sizeof(as));
    as.freq = 44100;
    as.format = AUDIO_S16;
    as.channels = 2;
    as.samples = 512;
    as.callback = &Sound::feed;
    as.userdata = this;
    if (SDL_OpenAudio(&as, nullptr) != 0) {
        return false;
    }

    SDL_PauseAudio(1); // set pause

    // load wave files
    if (!load(APP_DATADIR)) {
        // try portable variant
        char* app_dir = SDL_GetBasePath();
        if (app_dir) {
            std::string path = app_dir;
            path += "data";
            SDL_free(app_dir);
            load(path.c_str());
        }
    }

    return waves[0].data && waves[1].data;
}

void Sound::play(Sound::Type type)
{
    if (enable) {
        position = 0;
        current = &waves[type];
        if (current->data) {
            SDL_PauseAudio(0);
        }
    }
}

bool Sound::load(const char* dir)
{
    bool rc = true;

    for (size_t i = 0; i < sizeof(waves) / sizeof(waves[0]); ++i) {
        SDL_AudioSpec spec;
        Uint32 len;
        std::string file = dir;
        switch (i) {
            case Clatz:
                file += "clatz.wav";
                break;
            case Complete:
                file += "complete.wav";
                break;
        }
        if (SDL_LoadWAV(file.c_str(), &spec, &waves[i].data, &len)) {
            waves[i].size = len;
            rc |= true;
        }
    }

    return rc;
}

void Sound::feed(void* userdata, uint8_t* stream, int len)
{
    Sound* snd = reinterpret_cast<Sound*>(userdata);
    Wave& wav = *snd->current;

    const size_t remain = wav.size - snd->position;
    const Uint32 write = remain < static_cast<Uint32>(len) ? remain : len;
    memset(stream, 0, len);
    SDL_MixAudio(stream, &wav.data[snd->position], write, SDL_MIX_MAXVOLUME);
    snd->position += write;
    if (snd->position >= wav.size) {
        SDL_PauseAudio(1); // wave played, make pause
    }
}
