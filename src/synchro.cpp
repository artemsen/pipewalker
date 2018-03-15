/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
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

#include "synchro.h"

#ifndef WIN32
//Need to implement GetSystemTick() function in posix
#include <sys/time.h>
#endif // WIN32

unsigned int CSynchro::m_TickLast = 0;
unsigned int CSynchro::m_TickDelta = 0;
unsigned int CSynchro::m_TickStop = 0;


void CSynchro::Start()
{
	if (m_TickStop) {
		m_TickDelta = GetSystemTick() - m_TickStop;
		m_TickStop = 0;
	}
}


void CSynchro::Stop()
{
	if (!m_TickStop)
		m_TickStop = GetSystemTick();
}


unsigned int CSynchro::GetTick()
{
	if (!m_TickStop)
		m_TickLast = GetSystemTick() - m_TickDelta;

	return m_TickLast;
}


unsigned int CSynchro::GetSystemTick()
{
	unsigned int tick = 0;

#ifdef WIN32
	tick = GetTickCount();
#else
	//I hope it is posix...
	timeval tv;
	gettimeofday(&tv, NULL);
	tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif // WIN32

	return tick;
}


bool CSynchro::GetPhase(const unsigned int beginTime, const unsigned int maxTime, float& val)
{
	const unsigned int currTime = GetTick();
	const unsigned int diffTime = currTime - beginTime;

	if (diffTime > maxTime)
		return false;

	val = static_cast<float>(diffTime) / static_cast<float>(maxTime);
	return true;
}
