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
#include "button.h"
#include "map.h"


class CModeSettings
{
public:
	/**
	 * Initialization
	 */
	void Initialize();

	/**
	 * Render scene
	 * \param mouseX an X world mouse coordinate
	 * \param mouseY an Y world mouse coordinate
	 * \param transition transition phase [0...1]
	 * \return true if redraw is needed
	 */
	bool Render(const float mouseX, const float mouseY, const float transition);

	/**
	 * Mouse click handler
	 * \param button mouse button identifier
	 * \param mouseX world mouse X coordinate
	 * \param mouseY world mouse Y coordinate
	 */
	void OnMouseClick(const Uint8 button, const float mouseX, const float mouseY);

	/**
	 * Set default map size
	 * \param map size
	 */
	void SetMapSize(const MapSize mapSize);

	/**
	 * Get currently chosen map size
	 * \return map size
	 */
	MapSize GetMapSize() const;

private:
	vector<CRadioButton>	m_MapChoose;	///< Map size choosing buttons
};
