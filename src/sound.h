/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2012 by Artem Senichev <artemsen@gmail.com>        *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#pragma once

#include "common.h"


/**
 * Sound subsystem
 */
class sound
{
private:
	sound();

public:
	//! Sound types
	enum snd_type {
		clatz = 0, ///< Simple 'Clatz' sound
		complete,  ///< Level complete sound
		counter    ///< Sound counter
	};

	/**
	 * Get class instance
	 * \return class instance
	 */
	static sound& instance();

	/**
	 * Initialize sound subsystem
	 */
	void initialize();

	/**
	 * Play sound
	 * \param type sound identifier
	 */
	static void play(const snd_type type);

private:
	//! SDL fill audio buffer callback (see SDL SDK for more info)
	static void sdl_on_fill_buffer(void* userdata, Uint8* stream, int len);

private:
	//Wave sound description
	struct wav {
		vector<unsigned char> data; ///< Plain wave data
		size_t position;            ///< Current played position
	};

	wav      _bank[counter]; ///< Sound bank
	snd_type _curr_played;   ///< Currently played sound
	bool     _initialized;   ///< Sound subsystem initialization flag
};
