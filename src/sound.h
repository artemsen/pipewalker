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


/**
 * Sound wrapper
 */
class CSound
{
public:
	//Constructor/destructor
	CSound() : m_Data(NULL), m_Pos(0), m_Length(0) 	{}
	~CSound()										{ Free(); }

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
	Uint8*	m_Data;
	Uint32	m_Pos;
	Uint32	m_Length;
};


/**
 * Sound bank
 */
class CSoundBank
{
public:
	//! Sound types
	enum SoundType {
		SndClatz = 0,			///< Simple 'Clatz' sound
		SndCounter				///< sound counter
	};

	/**
	 * Initialize sound bank (load sounds from files)
	 */
	static void Initialize();

	/**
	 * Free sound bank
	 */
	static void Free();

	/**
	 * Play sound
	 * \param type sound identifier
	 */
	static void Play(const SoundType type);

private:
	//! SDL fill audio buffer callback (see SDL SDK for more info)
	static void OnFillBuffer(void* userdata, Uint8* stream, int len);

private:
	static CSound	m_Sound[SndCounter];	///< Sound bank
	static bool		m_UseSound;				///< Use sound flag
};
