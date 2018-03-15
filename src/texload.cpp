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

#include "texload.h"


bool CTextureLoader::LoadTexture(int* pnTexture, const char* pszFileName)
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

		fResult = LoadTexture(pnTexture, pImgData, nSize);
	}
	catch(const char* pszErr) {
		printf("Error loading textures from file %s: %s\n", pszFileName, pszErr);
	}

	if (pFile)
		fclose(pFile);

	if (pImgData)
		delete[] pImgData;

	return fResult;
}


#ifdef PWTARGET_WINNT
bool CTextureLoader::LoadTexture(int* pnTexture, unsigned int nIDRes)
{
	bool fResult = false;

	unsigned char* pImgData = NULL;
	HRSRC hRsrc = NULL;

	try {
		hRsrc = FindResource(NULL, MAKEINTRESOURCE(nIDRes), "TEXTURES");
		if (!hRsrc)
			throw("Resource not found");

		DWORD dwLen = SizeofResource(NULL, hRsrc);
		unsigned char* pRsrc = (unsigned char*)LoadResource(NULL, hRsrc);
		if (!pRsrc)
			throw("Failed to load resource");

		pImgData = new unsigned char[dwLen];
		if (pImgData == NULL)
			throw("Memory allocation error");

		memcpy(pImgData, pRsrc, dwLen);

		fResult = LoadTexture(pnTexture, pImgData, dwLen);
	}
	catch(const char* pszErr) {
		printf("Error loading textures from from resources: %s\n", pszErr);
	}

	if (pImgData)
		delete[] pImgData;

	if (hRsrc)
		FreeResource(hRsrc);

	return fResult;
}
#endif


bool CTextureLoader::LoadTexture(int* pnTexture, unsigned char* pImgData, const long nLength)
{
	Image hImage;

	//Image headers
	const char HdrTGAUnc[] = {0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	//Uncompressed TGA
	const char HdrTGARle[] = {0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	//RLE compressed TGA
	const char HdrBMP[]    = {0x42,0x4D};													//Bitmap (BMP)

	try {
		CBuffer imgBuf(pImgData, nLength);

		//Check header to define format
		if (memcmp(HdrTGAUnc, pImgData, sizeof(HdrTGAUnc)) == 0 ||
			memcmp(HdrTGARle, pImgData, sizeof(HdrTGARle)) == 0) {
			LoadTextureTGA(&imgBuf, &hImage);
		}
		else if (memcmp(HdrBMP, pImgData, sizeof(HdrBMP)) == 0) {
			LoadTextureBMP(&imgBuf, &hImage);
		}
		else {
			throw("Unknown format");
		}
	}
	catch(const char* pszErr) {
		printf("Error loading textures: %s\n", pszErr);
		return false;
	}

	//Create Texture
	glGenTextures(1, (GLuint*)pnTexture);
	glBindTexture(GL_TEXTURE_2D, (GLuint)*pnTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture

	glTexImage2D(GL_TEXTURE_2D, 0, hImage.Format, hImage.Width, hImage.Height, 0, hImage.Format, GL_UNSIGNED_BYTE, hImage.Data);

	return true;
}


void CTextureLoader::LoadTextureBMP(CBuffer* pBuf, Image* pImg)
{
	pImg->Format = GL_RGB;

	pBuf->GetData(18);	//Seek through the bmp header, up to the width/height

	//Read the width and height
	pImg->Width = pBuf->GetUINT32();
	pImg->Height = pBuf->GetUINT32();

	//Read the number of planes in image (must be 1)
	if (pBuf->GetUSORT16() != 1)
		throw("Planes from is not 1");

	//Read the number of bits per pixel (must be 24)
	if (pBuf->GetUSORT16() != 24)
		throw("BPP is not 24");

	//Seek past the rest of the bitmap header
	pBuf->GetData(24);

	//Calculate the size (assuming 24 bits or 3 bytes per pixel).
	unsigned long ulSize = pImg->Width * pImg->Height * 3;
	//assert(ulSize + nPos <= nLength);

	//Read the data.
	pImg->Data = const_cast<unsigned char *>(pBuf->GetData(ulSize));

	ConvertBGR2RGB(pImg->Data, ulSize, 3);
}


void CTextureLoader::LoadTextureTGA(CBuffer* pBuf, Image* pImg)
{
	pBuf->GetData(2);
	bool fUseRLE = (pBuf->GetUCHAR8() == 0x0A);
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
		throw("Incorrect format");

	//Calculate the number of bytes per pixel
	unsigned short int usBPP = pszHeader[4] / 8;

	//Calculate the size (assuming 24 bits or 3 or 4 bytes per pixel).
	unsigned long ulSize = pImg->Width * pImg->Height * usBPP;
	//assert(ulSize + nPos <= nLength);

	//Read the data
	if (!fUseRLE)
		pImg->Data = const_cast<unsigned char *>(pBuf->GetData(ulSize));
	else {
		//Create new own buffer to store image data
		pImg->Data = new unsigned char[ulSize];
		pImg->OwnBuffer = true;
		unsigned int nBufPos = 0;

		while (nBufPos < ulSize) {
			assert(pBuf->IsDataExist());
			int nPacketHeader = pBuf->GetUCHAR8();
			int nPacketLength = (nPacketHeader & 0x7F) + 1;
			if ((nPacketHeader & 0x80) != 0) {	// Run-length encoding packet (RLE)
				const unsigned char* pPacket = pBuf->GetData(usBPP);
				for (int j = 0; j < nPacketLength * usBPP; j++)
					pImg->Data[nBufPos++] = pPacket[j % usBPP];
			}
			else {								//Raw packet
				for (int j = 0; j < nPacketLength * usBPP; j++)
					pImg->Data[nBufPos++] = pBuf->GetUCHAR8();
			}
		}
	}
	ConvertBGR2RGB(pImg->Data, ulSize, usBPP);
}


void CTextureLoader::ConvertBGR2RGB(unsigned char* pImgData, const unsigned long nLength, const unsigned short nBPP)
{
	assert(nBPP == 3 || nBPP == 4);
	assert(nLength % nBPP == 0);

	for (unsigned long i = 0; i < nLength; i += nBPP) {
		unsigned char cTmp = pImgData[i];
		pImgData[i] = pImgData[i + 2];
		pImgData[i + 2] = cTmp;
	}
}
