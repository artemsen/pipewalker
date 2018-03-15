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
#include "button.h"
#include "explosion.h"
#include "texture.h"


class CGame
{
public:
	/**
	 * Start the game!
	 */
	static void StartGame();

private:
	//! Default constructor
	CGame();
	//! Default destructor
	~CGame() {}

	/**
	 * Do main game loop
	 */
	void DoMainLoop();

	/**
	 * Get mouse position in world coordinates
	 * \param posX returned X world mouse coordinate
	 * \param posY returned Y world mouse coordinate
	 * \return false if coordinates is out of the window
	 */
	static bool GetMousePosition(float& posX, float& posY);

	/**
	 * Post redraw event
	 */
	static void PostRedrawEvent();

	/**
	 * Render scene
	 */
	void RenderScene();

	/**
	 * Render environment
	 * \param mouseX world mouse X coordinate
	 * \param mouseY world mouse Y coordinate
	 * \return true if redraw is needed
	 */
	bool RenderEnvironment(const float mouseX, const float mouseY);

	/**
	 * Render puzzle
	 * \return true if redraw is needed
	 */
	bool RenderPuzzle();

	/**
	 * Render cell
	 * \param type texture type
	 */
	void RenderCell(const CTextureBank::TextureType type) const;

	/**
	 * Render information window
	 */
	void RenderInfo();

	/**
	 * Mouse click handler
	 * \param button mouse button identifier
	 */
	void OnMouseClick(const Uint8 button);

	//! Render modes
	enum Mode {
		InfoToPlay,	///< Transition from Info pane to play mode
		Play,		///< Play mode
		PlayToPlay,	///< Transition play to play (new game)
		PlayToInfo,	///< Transition from play mode to info pane
		Info		///< Info pane
	};

private:	//Class variables
	CMap					m_Map;			///< Game map
	CButton					m_BtnNext;
	CButton					m_BtnPrev;
	CButton					m_BtnReset;
	CButton					m_BtnInfo;
	CButton					m_BtnOk;
	list<CExplosion>		m_Explosions;

	unsigned int			m_NextMapId;

	Mode					m_Mode;
	unsigned int			m_ModeStartTime;
};
