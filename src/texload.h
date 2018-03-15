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


class CTextureLoader
{

public:

	/**
	 * Load texture from file
	 * @param pnTexture texture
	 * @param pszFileName a file name
	 * @return true if texture loaded
	 */
	static bool LoadTexture(int* pnTexture, const char* pszFileName);

#ifdef PWTARGET_WINNT
	/**
	 * Load texture from exe-file resources
	 * @param pnTexture texture
	 * @param nIDRes resource ID
	 * @return true if texture loaded
	 */
	static bool LoadTexture(int* pnTexture, unsigned int nIDRes);
#endif

	/**
	 * Load texture from memory
	 * @param pnTexture texture
	 * @param pszFileName a file name
	 * @return true if texture loaded
	 */
	static bool LoadTexture(int* pnTexture, unsigned char* pImgData, const long nLength);


private:

	//! Image description - contains height, width, data and format by picture
	struct Image {
		Image() : Width(0), Height(0), OwnBuffer(false), Data(0), Format(-1) {}
		~Image() { if (OwnBuffer && Data) delete[] Data; }
	    unsigned long	Width;		///< width of the image
	    unsigned long	Height;		///< height of the image
	    bool			OwnBuffer;	///< flag using own buffer to store image data
	    unsigned char*	Data;		///< image data
		int				Format;		///< format of the image (GL_RGB, GL_RGBA)
	};

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
	 * Bitmap loader (24 bit bitmaps with 1 plane only) (fuction trow exception if error)
	 * @param pBuf image data
	 * @param pImg a pointer to output Image structure
	 */
	static void LoadTextureBMP(CBuffer* pBuf, Image* pImg);

	/**
	 * TGA uncompressed loader (24 bit bitmaps with 1 plane only) (fuction trow exception if error)
	 * @param pBuf image data
	 * @param pImg a pointer to output Image structure
	 */
	static void LoadTextureTGA(CBuffer* pBuf, Image* pImg);

	/**
	 * Convert data from BGR to RGB mode
	 * @param pImgData image data
	 * @param nLength length of image data
	 * @param nBPP bots per pixel of image
	 */
	static void ConvertBGR2RGB(unsigned char* pImgData, const unsigned long nLength, const unsigned short nBPP);
};
