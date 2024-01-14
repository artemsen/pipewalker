// SPDX-License-Identifier: MIT
// Sound subsystem.
// Copyright (C) 2024 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstddef>
#include <cstdint>

/** Sound subsystem. */
class Sound {
public:
    ~Sound();

    /** Available sounds. */
    enum Type {
        Clatz,   ///< Rotation complete sound
        Complete ///< Level complete sound
    };

    /**
     * Initialization.
     * @return false if something went wrong
     */
    bool initialize();

    /**
     * Play specified sound.
     * @param type sound to play
     */
    void play(Type type);

    bool enable = true; ///< Sound enable/disable

private:
    /**
     * Load sound files from the specified directory.
     * @param dir path to directory with sound files (wav)
     * @return false if load failed
     */
    bool load(const char* dir);

    /** Callback that feeds the audio device. */
    static void feed(void* userdata, uint8_t* stream, int len);

    struct Wave {
        uint8_t* data; ///< Plain wave data
        size_t size;   ///< Size of wave data
    };
    Wave waves[2] = {};

    Wave* current;   ///< Currently played wave
    size_t position; ///< Current played position
};
