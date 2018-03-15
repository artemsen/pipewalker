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
#include "winmgr.h"
#include "map.h"

class CGame;

class CModeSettings
{
public:
	//! Default constructor
	CModeSettings(CGame& game) : m_Game(game) {}

	/**
	 * Initialization
	 */
	void Initialize();

	/**
	 * Reset internal state to setting values
	 */
	void Reset();

	/**
	 * Render scene
	 * \param mouseX an X world mouse coordinate
	 * \param mouseY an Y world mouse coordinate
	 * \param transition transition phase [0...1]
	 */
	void Render(const float mouseX, const float mouseY, const float transition);

	/**
	 * Mouse button down handler
	 * \param mouseX an X mouse world coordinate
	 * \param mouseY an Y mouse world coordinate
	 * \param btn mouse button identifier
	 */
	void OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn);

	/**
	 * Get currently chosen map size
	 * \return map size
	 */
	inline MapSize GetMapSize() const		{ return static_cast<MapSize>(m_MapSize.GetChoice()); }

	/**
	 * Get currently chosen wrapping mode
	 * \return wrapping mode
	 */
	inline bool GetWrapMode() const			{ return (m_WrapMode.GetChoice() == 1 ? true : false); }

	/**
	 * Get currently chosen sound mode
	 * \return sound mode
	 */
	inline bool GetSoundMode() const		{ return (m_Sound.GetChoice() == 1 ? true : false); }


private:
	CRadioButtons	m_MapSize;		///< Map size radio buttons group
	CRadioButtons	m_WrapMode;		///< Wrapping mode on/off radio buttons group
	CRadioButtons	m_Sound;		///< Sound on/off radio buttons group
	CGame&			m_Game;			///< Game instance
};
