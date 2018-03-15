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


class CButton
{
public:
	//! Default constructor
	CButton();

public:

	/**
	 * Create button
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width button width
	 * \param height button height
	 * \param tex button texture identifier
	 * \param id button ID
	 */
	void Create(const float x, const float y, const float width, const float height, const GLuint tex, const int id);

	/**
	 * Check for cross mouse and button coordinates
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 * \return true if mouse pointer is over button
	 */
	bool IsMouseOver(const float x, const float y) const;

	/**
	 * Render button
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 */
	void Render(const float x, const float y) const;

	/**
	 * Get button ID
	 * \return button ID
	 */
	int GetID() const	{ return m_id; }

private:	//Class variables
	float	m_x;		///< Button x coordinate
	float	m_y;		///< Button y coordinate
	float	m_width;	///< Button width
	float	m_height;	///< Button height
	GLuint	m_tex;		///< Button texture identifier
	int		m_id;		///< Button ID
};
