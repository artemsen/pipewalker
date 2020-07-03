// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

namespace sound {

/** @brief Sounds. */
enum Type {
    clatz,   ///< Rotation complete sound
    complete ///< Level complete sound
};

void initialize();
void finalize(); //?
void play(Type type);

} // namespace sound
