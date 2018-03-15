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

class level;


class explosion
{
public:
	/**
	 * Create winner animation
	 * \param lvl current level
	 */
	void create(const level& lvl);

	/**
	 * Free winner animation
	 */
	inline void free() { _particles.clear(); }

	/**
	 * Render explosion
	 */
	void draw(const float alpha);

private:
	//! Particles description
	struct particle {
		unsigned long life;   ///< Life time in ms
		unsigned long stime;  ///< Creation (start) time in ms
		float         x;      ///< Position by X axis
		float         y;      ///< Position by Y axis
		float         speed;  ///< Speed
		float         init_x; ///< Initial position by X axis
		float         init_y; ///< Initial position by Y axis
	};

private:
	particle create(const float x, const float y) const;

private:
	typedef vector<particle> particles;
	particles _particles; ///< Particle Array
};
