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


class CBuffer;

class CImage
{
public:
	//! Default constructor
	CImage();

	/**
	 * Copy constructor
	 * \param img existing image
	 */
	CImage(const CImage& img);

	//! Destructor
	~CImage();

	/**
	 * Load image from file
	 * \param fileName a file name
	 */
	void Load(const char* fileName);

	/**
	 * Image properties: Get image width
	 * \return image width
	 */
	inline size_t GetWidth() const				{ return m_ImgWidth; }

	/**
	 * Image properties: Get image height
	 * \return image height
	 */
	inline size_t GetHeight() const				{ return m_ImgHeight; }

	/**
	 * Image properties: Get image mode (GL_RGB / GL_RGBA / ...)
	 * \return image mode
	 */
	inline GLint GetMode() const				{ return m_ImgMode; }
	
	/**
	 * Image properties: Get image size in bytes
	 * \return image format
	 */
	inline size_t GetSize() const				{ return m_ImgWidth * m_ImgHeight * GetBPP(); }

	/**
	 * Image properties: Get number of bytes per pixel
	 * \return number byte per pixel
	 */
	inline unsigned char GetBPP() const			{ return m_ImgMode == GL_RGB ? 3 : m_ImgMode == GL_RGBA ? 4 : 1; }

	/**
	 * Image properties: Get image data
	 * \return image data (bitmap)
	 */
	inline unsigned char* const GetData() const	{ return m_Data; }


private:
	/**
	 * Targa image (TGA) loader
	 * \param buf image data
	 */
	void LoadTarga(CBuffer* buf);

	/**
	 * Bitmap image (BMP) loader
	 * \param buf image data
	 */
	void LoadBitmap(CBuffer* buf);

	/**
	 * Convert BGR to RGB mode
	 */
	void ConvertBGRtoRGB();

	/**
	 * Flip image by vertical
	 */
	void FlipVertical();

private:	//Class variables
	size_t			m_ImgWidth;		///< Image width
	size_t			m_ImgHeight;	///< Image height
	GLint			m_ImgMode;		///< Image mode
	unsigned char*	m_Data;			///< Image data
};
