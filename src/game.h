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
#include "map.h"
#include "button.h"
#include "modepuzzle.h"
#include "modesettings.h"
#include "settings.h"
#include "winmgr.h"


class CGame : public CEventHandler
{
public:
	//! Default constructor
	CGame();

	//! Destructor
	virtual ~CGame() {}

	/**
	 * Initialization
	 */
	void Initialize(CWinManager& winMgr);

	/**
	 * Finalize handler
	 */
	void Finalize();

public:
	//From CEventHandler
	void RenderScene(const float mouseX, const float mouseY);
	void OnKeyboardKeyDown(const char key);
	void OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn);
	void OnMouseMove(const float mouseX, const float mouseY);

private:
	//! Game render modes
	enum GameMode {
		Undefined,		///< Undefined mode
		Puzzle,			///< Puzzle (Play) mode
		Options,		///< Options mode
	};

	//! Transition phase
	enum TransitionPhase {
		FirstPhase,		///< First transition phase
		SecondPhase		///< Second transition phase
	};

	/**
	 * Renew map
	 * \param nextMapNum next map number
	 */
	void RenewMap(const unsigned long nextMapNum);

	/**
	 * Render environment
	 */
	void RenderEnvironment();

	/**
	 * Begin new transition
	 * \param nextMode next game render mode
	 * \param startPhase starting phase
	 */
	void BeginTransition(const GameMode nextMode, const TransitionPhase startPhase = FirstPhase);

	/**
	 * Check for transition in progress
	 * \return true if transition in progress
	 */
	inline bool TransitionInProgress() const	{ return (m_TrnStartTime != 0); }

public:
	//Accessors
	inline CWinManager& WinManager()	{ return *m_WinManager; }
	inline CSettings& Settings()		{ return m_Settings; }

private:	//Class variables
	CWinManager*	m_WinManager;		///< Window manager
	CSettings		m_Settings;			///< Game settings

	GameMode		m_ActiveMode;		///< Currently active mode
	GameMode		m_NextMode;			///< Next mode
	TransitionPhase	m_TrnPhase;			///< Transition phase (first=true, second=false)
	unsigned int	m_TrnStartTime;		///< Transition (mode changing) start time (zero if transition is not active)
	
	unsigned long	m_NextMapId;		///< Next map id (for next new game level)
	bool			m_RenewMap;			///< Map renew flag
	bool			m_LoadMap;			///< Map load flag

	CModePuzzle		m_ModePuzzle;		///< Renderer/handler (game mode)
	vector<CButton>	m_BtnPuzzle;		///< Buttons array (game mode)

	CModeSettings	m_ModeSettings;		///< Renderer/handler (settings mode)
	vector<CButton>	m_BtnSettings;		///< Buttons array (settings mode)
};
