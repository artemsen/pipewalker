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


class CBuffer;

class CImage
{
public:
	//! Default constructor
	CImage();

	/**
	 * Load image from file
	 * \param fileName a file name
	 */
	void Load(const char* fileName);

	/**
	 * Load XPM image
	 * \param data image data
	 * \param strNum string number in image data
	 */
	void LoadXPM(const char* data[], const size_t strNum);

	/**
	 * Image properties: Get image width
	 * \return image width
	 */
	inline size_t GetWidth() const				{ return _ImgWidth; }

	/**
	 * Image properties: Get image height
	 * \return image height
	 */
	inline size_t GetHeight() const				{ return _ImgHeight; }

	/**
	 * Image properties: Get image mode (GL_RGB / GL_RGBA / ...)
	 * \return image mode
	 */
	inline GLint GetMode() const				{ return _ImgMode; }

	/**
	 * Image properties: Get image size in bytes
	 * \return image format
	 */
	inline size_t SizeInBytes() const			{ return _ImgWidth * _ImgHeight * BytesPerPixel(); }

	/**
	 * Image properties: Get number of bytes per pixel
	 * \return number byte per pixel
	 */
	inline unsigned char BytesPerPixel() const	{ return _ImgMode == GL_RGB ? 3 : _ImgMode == GL_RGBA ? 4 : 1; }

	/**
	 * Image properties: Get image data
	 * \return image data (bitmap)
	 */
	inline const unsigned char* GetData() const	{ return _Data.empty() ? NULL : &_Data[0]; }

	/**
	 * Image properties: Get image data
	 * \return image data (bitmap)
	 */
	inline unsigned char* GetData()				{ return _Data.empty() ? NULL : &_Data[0]; }

	/**
	 * Get sub image
	 * \param x left corner coordinates
	 * \param y top coordinates
	 * \param width sub image width
	 * \param height sub image height
	 * \param data sub image array data
	 */
	void GetSubImage(const size_t x, const size_t y, const size_t width, const size_t height, vector<unsigned char>& data) const;

private:
	/**
	 * Convert BGR to RGB mode
	 */
	void ConvertBGRtoRGB();

	/**
	 * Flip image by vertical
	 */
	void FlipVertical();

private:	//Class variables
	size_t					_ImgWidth;	///< Image width
	size_t					_ImgHeight;	///< Image height
	GLint					_ImgMode;	///< Image mode
	vector<unsigned char>	_Data;		///< Image data
};
