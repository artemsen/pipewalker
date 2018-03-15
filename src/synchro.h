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

#include "common.h"


class CSynchro
{
public:
	//! Start timer
	static void Start();

	//! Stop timer
	static void Stop();

	/**
	 * Get current tick
	 * \return current tick count
	 */
	static unsigned int GetTick();

	/**
	 * Get phase (value in range [0...1])
	 * \param beginTime start tick value
	 * \param maxTime maximum tick value
	 * \param val value in range [0...1]
	 * \return false if time is up
	 */
	static bool GetPhase(const unsigned int beginTime, const unsigned int maxTime, float& val);

private:
	/**
	 * Get system time/tick counter (in ms)
	 * \return system time/tick counter
	 */
	static unsigned int GetSystemTick();

private:
	static unsigned int	_TickLast;		///< Last tick number
	static unsigned int	_TickDelta;	///< Delta tick number
	static unsigned int	_TickStop;		///< Stop time
};
