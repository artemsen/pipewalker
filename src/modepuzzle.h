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
#include "explosion.h"
#include "texture.h"
#include "winmgr.h"
#include "map.h"

class CGame;

class CModePuzzle
{
public:
	/**
	 * Constructor
	 * \param game the game instance
	 */
	CModePuzzle(CGame& game) : _Game(game), _UseSound(true) {}

	/**
	 * Renew map
	 */
	void RenewMap();

	/**
	 * Load saved map (from settings)
	 */
	void LoadMap();

	/**
	 * Save map to settings
	 */
	void SaveMap() const;

	/**
	 * Render scene
	 * \param transition transition phase [0...1]
	 */
	void Render(const float transition);

	/**
	 * Mouse button down handler
	 * \param mouseX an X mouse world coordinate
	 * \param mouseY an Y mouse world coordinate
	 * \param btn mouse button identifier
	 */
	void OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn);

	/**
	 * Reset by rotate current map
	 */
	void ResetByRotate()		{ StopWinnerAnimation(); _UseSound = false; _Map.ResetByRotate(); }

	/**
	 * Get current map size
	 * \return current map size
	 */
	MapSize GetMapSize() const	{ return _Map.GetMapSize(); }

private:
	/**
	 * Stop winner animation
	 */
	inline void StopWinnerAnimation()	{ _Explosions.clear(); }

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
	inline float GetMapScaleFactor() const	{ return (10.0f / static_cast<float>(_Map.GetMapSize())); }

private:
	CGame&				_Game;			///< Game instance
	CMap				_Map;			///< Game map
	list<CExplosion>	_Explosions;	///< Winner explosions
	bool				_UseSound;		///< Use sound flag (to prevent 'reset by rotate' sounds)
};
