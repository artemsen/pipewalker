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
#include "texture.h"
#include "rendertext.h"
#include "sound.h"
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
	void ReloadTextures();

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
	inline bool TransitionInProgress() const	{ return (_TrnStartTime != 0); }

public:
	//Accessors
	inline CWinManager& WinManager()			{ return *_WinManager; }
	inline CUserSettings& UserSettings()		{ return _UserSettings; }
	inline const CGameSettings& GameSettings() const { return _GameSettings; }
	inline CTextureBank& TextureBank()				{ return _TextureBank; }
	inline const CRenderText& RenderText() const	{ return _RenderText; }
	inline CSoundBank& SoundBank()					{ return _SoundBank; }

private:	//Class variables
	CWinManager*	_WinManager;		///< Window manager
	CUserSettings	_UserSettings;		///< Game settings
	CGameSettings	_GameSettings;		///< Game settings
	CTextureBank	_TextureBank;		///< Texture bank
	CRenderText		_RenderText;		///< Text renderer
	CSoundBank		_SoundBank;			///< Sound bank

	GameMode		_ActiveMode;		///< Currently active mode
	GameMode		_NextMode;			///< Next mode
	TransitionPhase	_TrnPhase;			///< Transition phase (first=true, second=false)
	unsigned int	_TrnStartTime;		///< Transition (mode changing) start time (zero if transition is not active)
	
	unsigned long	_NextMapId;			///< Next map id (for next new game level)
	bool			_RenewMap;			///< Map renew flag
	bool			_LoadMap;			///< Map load flag

	CModePuzzle		_ModePuzzle;		///< Renderer/handler (game mode)
	list<CButton>	_BtnPuzzle;			///< Buttons array (game mode)

	CModeSettings	_ModeSettings;		///< Renderer/handler (settings mode)
	list<CButton>	_BtnSettings;		///< Buttons array (settings mode)
};
