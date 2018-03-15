/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
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

#define MTSTATE_ARRAY_SIZE 624


class CMTRandom
{
public:
	/**
	 * Seed random sequence
	 * \param seed number to seed
	 */
	static void Seed(const unsigned long seed);

	/**
	 * Get random number
	 * \return random number
	 */
	static unsigned long Rand();

private:
	/**
	 * Number twiddler
	 */
	static unsigned long Twiddle(const unsigned long u, const unsigned long v);

	/**
	 * State generator
	 */
	static void GenerateState();

private:
	static unsigned long	m_State[MTSTATE_ARRAY_SIZE];	///< State array
	static unsigned long	m_Pos;							///< Position in state array
	static bool				m_Initialized;					///< Initialization flag
};
