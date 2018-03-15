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

class CImage;

/**
 * Texture wrapper
 */
class CTexture
{
public:
	//Constructor/destructor
	CTexture() : m_Id(0)	{}
	~CTexture()				{ Free(); }

	/**
	 * Free texture
	 */
	void Free();

	/**
	 * Get texture identifier
	 * \return texture identifier
	 */
	GLuint GetId() const	{ return m_Id; }

	/**
	 * Create texture from part of the image
	 * \param img source image
	 * \param x an X coordinate of texture
	 * \param y an Y coordinate of texture
	 * \param width subimage width
	 * \param height subimage height
	 * \param modeWrap wrap texture mode
	 */
	void Load(const CImage& img, const size_t x, const size_t y, const size_t width, const size_t height, const int modeWrap = GL_CLAMP);

	/**
	 * Load texture from file
	 * \param fileName texture file name
	 * \param modeWrap default wrap texture mode
	 */
	void Load(const char* fileName, const int modeWrap = GL_CLAMP);

private:
	GLuint	m_Id;				///< Texture identifier
};


/**
 * Texture bank
 */
class CTextureBank
{
public:
	//! Textures types
	enum TextureType {
		TexEnvBkgr = 0,			///< Main environment background
		TexEnvTitle,			///< Main environment title
		TexCellBackground,		///< Cell background
		TexSender,				///< Sender
		TexReceiverActive,		///< Active receiver
		TexReceiverPassive,		///< Passive receiver
		TexLock,				///< Lock
		TexTubeHalfPassive,		///< Passive half tube
		TexTubeHalfActive,		///< Active half tube
		TexTubeCrvPassive,		///< Passive curved tube
		TexTubeCrvActive,		///< Active curved tube
		TexTubeStrPassive,		///< Passive straight tube
		TexTubeStrActive,		///< Active straight tube
		TexTubeJnrPassive,		///< Passive joiner tube
		TexTubeJnrActive,		///< Active joiner tube
		TexExplosionPart,		///< Explosion particle
		TexNum0,				///< Number 0
		TexNum1,				///< Number 1
		TexNum2,				///< Number 2
		TexNum3,				///< Number 3
		TexNum4,				///< Number 4
		TexNum5,				///< Number 5
		TexNum6,				///< Number 6
		TexNum7,				///< Number 7
		TexNum8,				///< Number 8
		TexNum9,				///< Number 9
		TexButtonNext,			///< Button 'next' (new game)
		TexButtonPrev,			///< Button 'previus'
		TexButtonReset,			///< Button 'reset'
		TexButtonSett,			///< Button 'settings'
		TexButtonOK,			///< Button 'OK'
		TexButtonCancel,		///< Button 'Cancel'
		TexRadBtnOn,			///< Radio button 'On'
		TexRadBtnOff,			///< Radio button 'Off'
		TexCounter				///< texture counter
	};

	/**
	 * Initialize texture bank (load textures from files)
	 */
	static void Load();

	/**
	 * Free texture bank
	 */
	static void Free();

	/**
	 * Get texture identifier
	 * \param type model type
	 * \return texture identifier
	 */
	static GLuint Get(const TextureType type);

private:
	static CTexture	m_Texture[TexCounter];	///< Textures
};
