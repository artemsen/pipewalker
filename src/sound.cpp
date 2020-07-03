// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "settings.hpp"
#include "sound.hpp"

#include <SDL2/SDL.h>
#include <cassert>
#include <vector>

namespace sound {

/** @brief Wave descruption. */
struct Wave {
    const char* file;          ///< File to load
    std::vector<uint8_t> data; ///< Plain wave data
    size_t position;           ///< Current played position
};

/** @brief Sound bank. */
static Wave waves[] = { { GAME_DATA_DIR "/clatz.wav", {}, 0 }, { GAME_DATA_DIR "/complete.wav", {}, 0 } };
static_assert(sizeof(waves) / sizeof(waves[0]) == complete + 1);
/** @brief Currently played sound. */
static size_t current_wav;

static void fill_buffer(void* /*userdata*/, Uint8* stream, int len)
{
    assert(current_wav <= sizeof(waves) / sizeof(waves[0]));
    Wave& wav = waves[current_wav];
    assert(!wav.data.empty());

    assert(wav.position < wav.data.size());

    SDL_memset(stream, 0, len);

    const size_t remain = wav.data.size() - wav.position;
    const Uint32 write = remain < static_cast<Uint32>(len) ? remain : len;
    SDL_MixAudio(stream, &wav.data[wav.position], write, SDL_MIX_MAXVOLUME);
    wav.position += write;
    if (wav.position >= wav.data.size()) {
        SDL_PauseAudio(1); // wave played, make pause
    }
}

void initialize()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return;
    }
    SDL_AudioSpec as;
    std::fill(reinterpret_cast<uint8_t*>(&as), reinterpret_cast<uint8_t*>(&as) + sizeof(as), 0);
    as.freq = 44100;
    as.format = AUDIO_S16;
    as.channels = 2;
    as.samples = 512;
    as.callback = fill_buffer;
    as.userdata = nullptr;
    if (SDL_OpenAudio(&as, nullptr) != 0) {
        return;
    }

    SDL_PauseAudio(1); // Set pause

    // load wave files
    for (size_t i = 0; i < sizeof(waves) / sizeof(waves[0]); ++i) {
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 len;
        if (SDL_LoadWAV(waves[i].file, &spec, &data, &len)) {
            waves[i].data.reserve(len);
            std::copy(data, data + len, std::back_inserter(waves[i].data));
            SDL_FreeWAV(data);
        }
    }
}

void finalize() { }

void play(Type type)
{
    if (!settings::instance().sound) {
        return; // sound is disabled
    }

    current_wav = type;
    Wave& wav = waves[current_wav];
    if (wav.data.empty()) {
        return; // not intialized
    }

    wav.position = 0;
    SDL_PauseAudio(0);
}

} // namespace sound
