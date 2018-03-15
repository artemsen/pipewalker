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

#include "tgaloader.h"


bool CTGALoader::LoadTexture(GLuint* pnTexture, const char* pszFileName, char* pszErrorDescr)
{
	Image hImage;
	if (LoadImage(&hImage, pszFileName, pszErrorDescr)) {
		//Create Texture
		glBindTexture(GL_TEXTURE_2D, *pnTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
		glTexImage2D(GL_TEXTURE_2D, 0, hImage.Format, hImage.Width, hImage.Height, 0, hImage.Format, GL_UNSIGNED_BYTE, hImage.Data);
		return true;
	}
	return false;
}


bool CTGALoader::LoadImage(Image* pImage, const char* pszFileName, char* pszErrorDescr)
{
	bool fResult = false;

	unsigned char* pImgData = NULL;
	FILE* pFile = NULL;

	try {
		pFile = fopen(pszFileName, "rb");
		if (pFile == NULL)
			throw("File not found");

		fseek(pFile, 0, SEEK_END);
		long nSize = ftell(pFile);
		if (nSize < 32)
			throw("Incorrect format");
		fseek(pFile, 0, SEEK_SET);

		pImgData = new unsigned char[nSize];
		if (pImgData == NULL)
			throw("Memory allocation error");

		if (fread(pImgData, nSize, 1, pFile) != 1)
			throw("Error reading file");

		CBuffer imgBuf(pImgData, nSize);
		LoadTGA(&imgBuf, pImage);
		
		fResult = true;
	}
	catch(const char* pszErr) {
		sprintf(pszErrorDescr, "Error loading textures from file \"%s\": %s\n", pszFileName, pszErr);
		fResult = false;
	}

	if (pFile)
		fclose(pFile);

	if (pImgData)
		delete[] pImgData;

	return fResult;
}


void CTGALoader::LoadTGA(CBuffer* pBuf, Image* pImg)
{
	pBuf->GetData(2);
	unsigned char nFormat = pBuf->GetUCHAR8();
	bool fUseRLE;
	if (nFormat == 0x0A)
		fUseRLE = true;			//RLE compressed TGA
	else if (nFormat == 0x02)
		fUseRLE = false;		//Uncompressed TGA
	else
		throw("Incorrect format (unknown RLE flag in header)");
	pBuf->GetData(9);	//Seek through the TGA header, up to the First 6 Useful Bytes From The Header

	const unsigned char* pszHeader = pBuf->GetData(6);

	//Read the width and height
	pImg->Width  = pszHeader[1] * 256 + pszHeader[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	pImg->Height = pszHeader[3] * 256 + pszHeader[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
 	if (pImg->Width	<=0 || pImg->Height <=0)
		throw("Incorrect format");

	//Define format
	if (pszHeader[4] == 24)
		pImg->Format = GL_RGB;
	else if (pszHeader[4] == 32)
		pImg->Format = GL_RGBA;
	else
		throw("Incorrect format (color type unsupported)");

	//Calculate the number of bytes per pixel
	unsigned int usBPP = pszHeader[4] / 8;

	//Calculate the size (assuming 24 bits or 3 or 4 bytes per pixel).
	unsigned long ulSize = pImg->Width * pImg->Height * usBPP;
	//assert(ulSize + nPos <= nLength);

	//Read the data
	pImg->Data = new unsigned char[ulSize];
	if (!fUseRLE)
		memcpy(pImg->Data, pBuf->GetData(ulSize), ulSize); 
	else {
		//Create new own buffer to store image data
		unsigned int nBufPos = 0;

		while (nBufPos < ulSize) {
			if (!pBuf->IsDataExist())
				throw("Incorrect format (not enought data)");
			int nPacketHeader = pBuf->GetUCHAR8();
			int nPacketLength = (nPacketHeader & 0x7F) + 1;
			if ((nPacketHeader & 0x80) != 0) {	// Run-length encoding packet (RLE)
				const unsigned char* pPacket = pBuf->GetData(usBPP);
				for (unsigned int j = 0; j < nPacketLength * usBPP; j++)
					pImg->Data[nBufPos++] = pPacket[j % usBPP];
			}
			else {								//Raw packet
				for (unsigned int j = 0; j < nPacketLength * usBPP; j++)
					pImg->Data[nBufPos++] = pBuf->GetUCHAR8();
			}
		}
	}
	//Convert BGR to RGB
	for (unsigned long i = 0; i < ulSize; i += usBPP) {
		unsigned char cTmp = pImg->Data[i];
		pImg->Data[i] = pImg->Data[i + 2];
		pImg->Data[i + 2] = cTmp;
	}
}
