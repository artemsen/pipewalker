/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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


//! Image description - contains height, width, data and format by picture
struct Image {
	Image() : Width(0), Height(0), Data(NULL), Format(-1) {}
	~Image() { if (Data) delete[] Data; }
	unsigned long	Width;		///< width of the image
	unsigned long	Height;		///< height of the image
	unsigned char*	Data;		///< image data
	int				Format;		///< format of the image (GL_RGB, GL_RGBA)
};


class CTGALoader
{

public:

	/**
	 * Load texture from file
	 * @param pnTexture texture
	 * @param pszFileName a file name
	 * @param pszErrorDescr an output buffer to store error message
	 * @return true if texture loaded
	 */
	static bool LoadTexture(GLuint* pnTexture, const char* pszFileName, char* pszErrorDescr);

	/**
	 * Load image from file
	 * @param pImgData pointer to output structure
	 * @param pszFileName a file name
	 * @param pszErrorDescr an output buffer to store error message
	 * @return true if image loaded
	 */
	static bool LoadImage(Image* pImage, const char* pszFileName, char* pszErrorDescr);


private:
	//! Class to work with image data buffer
	class CBuffer
	{
	public:
		CBuffer(const unsigned char* pData, const int nLength) : m_pData(pData), m_nLength(nLength), m_nPos(0) {}
		const unsigned char* GetData(const int nLength)	{ const unsigned char* pRet = m_pData + m_nPos;	m_nPos += nLength; return pRet; }
		const unsigned int GetUINT32()					{ return *(unsigned int*)(GetData(sizeof(unsigned int)));}
		const unsigned short int GetUSORT16()			{ return *(unsigned short int*)(GetData(sizeof(unsigned short int)));}
		const unsigned char GetUCHAR8()					{ return *(unsigned char*)(GetData(sizeof(unsigned char)));}
		bool IsDataExist() const						{ return m_nPos < m_nLength; }
	private:
		const unsigned char*	m_pData;
		int						m_nLength;
		int						m_nPos;
	};

	/**
	 * TGA loader (24 bit bitmaps with 1 plane only) (fuction trow exception if error)
	 * @param pBuf image data
	 * @param pImg a pointer to output Image structure
	 */
	static void LoadTGA(CBuffer* pBuf, Image* pImg);
};
