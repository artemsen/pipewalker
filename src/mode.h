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

//Main button position
#define PW_BUTTON_Y  -5.6f
#define PW_BUTTON_X1 -4.5f
#define PW_BUTTON_X2 -3.0f
#define PW_BUTTON_X3  3.0f
#define PW_BUTTON_X4  4.5f


/**
 * Mode interface
 */
class mode
{
public:
	/**
	 * Draw scene
	 * \param alpha transparency value in range [0.0f...1.0f]
	 * \return true if we need to redraw window
	 */
	virtual bool draw(const float alpha) = 0;

	/**
	 * Mouse move handler
	 * \param x position of the mouse cursor on the X axis
	 * \param y position of the mouse cursor on the Y axis
	 * \return true if we need to redraw window
	 */
	virtual bool on_mouse_move(const float x, const float y) = 0;

	/**
	 * Mouse click handler
	 * \param x position of the mouse cursor on the X axis
	 * \param y position of the mouse cursor on the Y axis
	 * \param btn mouse button identifier
	 * \return true if mode must be swapped
	 */
	virtual bool on_mouse_click(const float x, const float y, const Uint8 btn) = 0;
};
