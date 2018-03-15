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

#pragma once

#include "common.h"
#include "texture.h"


class CRenderText
{
public:
	/**
	 * Load font data
	 */
	static void Load();

	/**
	 * Free font data
	 */
	static void Free();

	/**
	 * Print string
	 * \param type font type
	 * \param x an X coordinate of top left text corner (window coordinates begin from top left)
	 * \param y an Y coordinate of top left text corner (window coordinates begin from top left)
	 * \param scale scale coefficient
	 * \param color[4] pointer to RGBA text color value
	 * \param shadow draw shadow flag
	 * \param text output string
	 */
	static void Print(const float x, const float y, const float scale, const float color[4], const bool shadow, const char* text);


private:	//Class variables
	static GLuint	m_DispList;		///< First display list identifier for normal font
	static CTexture	m_Texture;		///< Fonts texture
};
