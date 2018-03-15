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

#define MTR_STATE_ARRAY_SIZE 624


class mtrandom
{
private:
	mtrandom();

public:
	/**
	 * Get class instance
	 * \return class instance
	 */
	static mtrandom& instance();

	/**
	 * Seed random sequence
	 * \param seed number to seed
	 */
	static void seed(const unsigned long seed);

	/**
	 * Get random number in range [min_val...max_val)
	 * \param min_val minimal value
	 * \param max_val maximum value
	 * \return random number
	 */
	static float random(const float min_val, const float max_val);

	/**
	 * Get random number in range [min_val...max_val)
	 * \param min_val minimal value
	 * \param max_val maximum value
	 * \return random number
	 */
	static int random(const int min_val, const int max_val);

private:
	/**
	 * State generator
	 */
	void generate_state();

	/**
	 * Get random number
	 * \return random number
	 */
	unsigned long random();

private:
	unsigned long _state[MTR_STATE_ARRAY_SIZE]; ///< State array
	unsigned long _pos;                         ///< Position in the state array
};
