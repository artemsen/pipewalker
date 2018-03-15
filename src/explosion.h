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


class CExplosion
{
public:
	/**
	 * Constructor
	 * \param x an initial X explosion position
	 * \param y an initial Y explosion position
	 */
	CExplosion(const float x, const float y);

	/**
	 * Render explosion
	 */
	void Render();

private:
	/**
	 * Renew explosion
	 */
	void Renew();

private:
	//! Particles description
	struct PARTICLE {
		float	Life;		///< Life value
		float	FadeSpeed;	///< Fade speed
		float	PosX;		///< Position by X axis
		float	PosY;		///< Position by Y axis
		float	SpeedX;		///< Speed by X axis
		float	SpeedY;		///< Speed by Y axis
	};

	vector<PARTICLE>	m_Particles;	///< Particle Array
	float				m_X;			///< Initial X explosion position
	float				m_Y;			///< Initial Y explosion position
	float				m_Force;		///< Explosion force
};
