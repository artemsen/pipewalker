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
#include "modepuzzle.h"
#include "modesettings.h"


class CGame
{
public:
	//! Default constructor
	CGame();

	/**
	 * Do main game loop
	 */
	void DoMainLoop();

private:
	//! Render modes
	enum Mode {
		Puzzle,				///< Puzzle (Play) mode
		Puzzle2Puzzle,		///< Puzzle to puzzle transition mode
		Puzzle2Settings,	///< Puzzle to settings transition mode
		Settings,			///< Settings mode
		Settings2Puzzle		///< Settings to puzzle transition mode
	};

	//! Transition phase
	enum Phase {
		FirstPhase,			///< First transition phase
		SecondPhase			///< Second transition phase
	};

	/**
	 * Get mouse position in world coordinates
	 * \param mouseX returned X world mouse coordinate
	 * \param mouseY returned Y world mouse coordinate
	 * \return false if coordinates is out of the window
	 */
	static bool GetMousePosition(float& mouseX, float& mouseY);

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
	 */
	void RenderEnvironment();

	/**
	 * Mouse click handler
	 * \param button mouse button identifier
	 */
	void OnMouseClick(const Uint8 button);

	/**
	 * Begin new transition
	 * \param nextMode next expected mode
	 */
	void BeginTransition(const Mode nextMode);

private:	//Class variables
	Mode			m_Mode;				///< Currently active mode
	Phase			m_TrnPhase;			///< Transition phase (first=true, second=false)
	unsigned int	m_TrnStartTime;		///< Transition (mode changing) start time (zero if transition is not in active mode)

	CMap			m_Map;				///< Game map
	unsigned int	m_NextMapId;		///< Next map id (for next new game level)

	CModePuzzle		m_ModePuzzle;		///< Renderer/handler (game mode)
	vector<CButton>	m_BtnPuzzle;		///< Buttons array (game mode)

	CModeSettings	m_ModeSettings;		///< Renderer/handler (settings mode)
	vector<CButton>	m_BtnSettings;		///< Buttons array (settings mode)
};
