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

#if defined PW_USE_SDL		//SDL library
	#include <SDL/SDL.h>
#endif

/**
 * Sound wrapper
 */
class CSound
{
public:
	//Constructor/destructor
	CSound();
	~CSound();

	/**
	 * Free sound
	 */
	void Free();

	/**
	 * Load Wav sound from file
	 * \param fileName sound file name
	 */
	void Load(const char* fileName);

public:
#if defined PW_USE_SDL		//SDL library
	size_t	_Pos;
#endif
	vector<unsigned char> _Data;
};


/**
 * Sound bank
 */
class CSoundBank
{
public:
	//Constructor/Destructor
	CSoundBank();
	~CSoundBank();

	//! Sound types
	enum SoundType {
		SndClatz = 0,			///< Simple 'Clatz' sound
		SndComplete,			///< Level complete sound
		SndCounter				///< sound counter
	};

	/**
	 * Initialize sound bank (load sounds from files)
	 */
	void Load();

	/**
	 * Play sound
	 * \param type sound identifier
	 */
	void Play(const SoundType type);

private:
	/**
	 * Free sound bank
	 */
	void Free();

#ifdef PW_USE_SDL
	//! SDL fill audio buffer callback (see SDL SDK for more info)
	static void OnFillBuffer(void* userdata, Uint8* stream, int len);
	static SoundType _CurrentSnd;	///< Current played sound
#endif // PW_USE_SDL

private:
	CSound	_Sound[SndCounter];	///< Sound bank
	bool	_SoundInitialized;	///< Sound subsystem initialization flag
};
