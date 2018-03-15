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
#include "map.h"
#include "explosion.h"
#include "texture.h"


class CModePuzzle
{
public:
	//! Default constructor
	CModePuzzle(CMap* gameMap) : m_Map(gameMap) {}

	/**
	 * Render scene
	 * \param transition transition phase [0...1]
	 * \return true if redraw is needed
	 */
	bool Render(const float transition);

	/**
	 * Mouse click handler
	 * \param button mouse button identifier
	 * \param mouseX world mouse X coordinate
	 * \param mouseY world mouse Y coordinate
	 */
	void OnMouseClick(const Uint8 button, const float mouseX, const float mouseY);

	/**
	 * Reset explosions
	 */
	void ResetExplosions()	{ m_Explosions.clear(); }

private:
	/**
	 * Render puzzle
	 * \param transition transition phase [0...1]
	 * \return true if redraw is needed
	 */
	bool RenderPuzzle(const float transition);

	/**
	 * Render cell
	 * \param type texture type
	 */
	void RenderCell(const CTextureBank::TextureType type) const;

	/**
	 * Get map scale factor
	 * \return map scale factor
	 */
	float GetMapScaleFactor() const				{ return (10.0f / static_cast<float>(m_Map->GetMapSize())); }

private:
	vector<CExplosion>	m_Explosions;	///< Winner explosions
	CMap*				m_Map;			///< Game map
};
