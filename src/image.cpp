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

#include "image.h"
#include "buffer.h"

//Image specific headers
#pragma pack (push, 1)
//Targa
struct IMAGE_TGA_HEADER {
	unsigned char	Length;
	unsigned char	ColormapType;
	unsigned char	ImageType;
	unsigned short	ColormapIndex;
	unsigned short	ColormapLength;
	unsigned char	ColormapEntrySize;
	unsigned short	OriginX;
	unsigned short	OriginY;
	unsigned short	Width;
	unsigned short	Height;
	unsigned char	PixelSize;
	unsigned char	ImageDesc;
};
#define IMAGE_TGA_RLE		0x0A	//RLE compressed TGA
#define IMAGE_TGA_RGB		0x02	//Uncompressed TGA

//Bitmap
struct IMAGE_BMP_HEADER {
	unsigned short	Type;
	unsigned long	Size;
	unsigned short	Reserved1;
	unsigned short	Reserved2;
	unsigned long	OffBits;
};

struct IMAGE_BMP_INFO {
	unsigned long	Size;
	unsigned long	Width;
	unsigned long	Height;
	unsigned short	Planes;
	unsigned short	BitCount;
	unsigned long	Compression;
	unsigned long	SizeImage;
	unsigned long	XPelsPerMeter;
	unsigned long	YPelsPerMeter;
	unsigned long	ClrUsed;
	unsigned long	ClrImportant;
};

#define IMAGE_BMP_MAGIC		0x4D42	//Magic BMP header

#pragma pack (pop)


CImage::CImage()
:	m_ImgWidth(0),
	m_ImgHeight(0),
	m_ImgMode(GL_RGB),
	m_Data(NULL)
{
}


CImage::CImage(const CImage& img)
:	m_ImgWidth(img.GetHeight()),
	m_ImgHeight(img.GetWidth()),
	m_ImgMode(img.GetMode()),
	m_Data(NULL)
{
	const size_t size = GetSize();
	m_Data = new unsigned char[size];
	memcpy(m_Data, img.GetData(), size);
}


CImage::~CImage()
{
	delete[] m_Data;
}


void CImage::Load(const char* fileName)
{
	assert(fileName);
	CBuffer buf;
	buf.Load(fileName);
	if (strstr(fileName, ".tga"))
		LoadTarga(&buf);
	else
		LoadBitmap(&buf);
}


void CImage::LoadTarga(CBuffer* buf)
{
	assert(buf);
	assert(m_Data == NULL);

	const IMAGE_TGA_HEADER* header = reinterpret_cast<const IMAGE_TGA_HEADER*>(buf->GetData(sizeof(IMAGE_TGA_HEADER)));
	if (!header)
		throw CException("TGA: Incorrect format");

	if (header->ColormapType != 0)
		throw CException("TGA: Colormap not supported");

	if (header->ImageType != IMAGE_TGA_RLE && header->ImageType != IMAGE_TGA_RGB)
		throw CException("TGA: Only RLE or RGB mode supported");

	//Read the width and height
	m_ImgWidth  = header->Width;
	m_ImgHeight = header->Height;
 	if (m_ImgWidth == 0 || m_ImgHeight == 0 || m_ImgWidth > 4096 || m_ImgHeight > 4096)
		throw CException("TGA: Undefined image size");

	//Define format
	if (header->PixelSize == 24)
		m_ImgMode = GL_RGB;
	else if (header->PixelSize == 32)
		m_ImgMode = GL_RGBA;
	else
		throw CException("TGA: Pixel size unsupported");

	//Calculate the number of bytes per pixel
	const size_t bpp = header->PixelSize / 8;

	//Calculate the size
	const size_t sizeInBytes = m_ImgWidth * m_ImgHeight * bpp;

	//Read the data
	m_Data = new unsigned char[sizeInBytes];
	if (header->ImageType == IMAGE_TGA_RGB)
		memcpy(m_Data, buf->GetData(sizeInBytes), sizeInBytes);
	else {
		//Create new own buffer to store image data
		size_t bufPos = 0;

		while (bufPos < sizeInBytes) {
			unsigned char packetHeader;
			if (!buf->Get(packetHeader))
				throw CException("TGA: Incorrect format (not enough data)");
			const unsigned char packetLength = (packetHeader & 0x7F) + 1;
			if ((packetHeader & 0x80) != 0) {	// Run-length encoding packet (RLE)
				const unsigned char* packet = reinterpret_cast<const unsigned char*>(buf->GetData(bpp));
				for (Uint32 j = 0; j < packetLength * bpp; ++j) {
					if (bufPos >= sizeInBytes)
						throw CException("TGA: Incorrect format");
					m_Data[bufPos++] = packet[j % bpp];
				}
			}
			else {								//Raw packet
				for (size_t j = 0; j < packetLength * bpp; ++j) {
					if (bufPos >= sizeInBytes || !buf->Get(m_Data[bufPos++]))
						throw CException("TGA: Incorrect format");
				}
			}
		}
	}

	ConvertBGRtoRGB();

	if ((header->ImageDesc & 0x20) != 0)
		FlipVertical();
}


void CImage::LoadBitmap(CBuffer* buf)
{
	assert(buf);
	assert(m_Data == NULL);

	const IMAGE_BMP_HEADER* pBMPFileHeader = reinterpret_cast<const IMAGE_BMP_HEADER*>(buf->GetData(sizeof(IMAGE_BMP_HEADER)));
	const IMAGE_BMP_INFO* pBMPInfoHeader = reinterpret_cast<const IMAGE_BMP_INFO*>(buf->GetData(sizeof(IMAGE_BMP_INFO)));

	if (pBMPFileHeader->Type != IMAGE_BMP_MAGIC)
		throw CException("unknown format (header magic number undefined)");

	if (pBMPInfoHeader->Planes != 1)
		throw CException("only 1 plane supported");

	//Read the width and height
	m_ImgWidth  = pBMPInfoHeader->Width;
	m_ImgHeight = pBMPInfoHeader->Height;
 	if (m_ImgWidth == 0 || m_ImgHeight == 0 || m_ImgWidth > 4096 || m_ImgHeight > 4096)
		throw CException("undefined image size");

	//Define format
	if (pBMPInfoHeader->BitCount == 24)
		m_ImgMode = GL_RGB;
	else if (pBMPInfoHeader->BitCount == 32)
		m_ImgMode = GL_RGBA;
	else
		throw CException("pixel size unsupported");

	//Calculate the number of bytes per pixel
	const size_t usBPP = pBMPInfoHeader->BitCount / 8;

	//Calculate the size
	const size_t ulSize = m_ImgWidth * m_ImgHeight * usBPP;

	//Read the data
	if (!buf->SetOffset(pBMPFileHeader->OffBits))
		throw CException("incorrect format");
	m_Data = new unsigned char[ulSize];
	memcpy(m_Data, buf->GetData(ulSize), ulSize);

	ConvertBGRtoRGB();
}


void CImage::ConvertBGRtoRGB()
{
	assert(m_Data != NULL);
	assert(m_ImgMode == GL_RGB || m_ImgMode == GL_RGBA);
	if (m_ImgMode != GL_RGB && m_ImgMode != GL_RGBA)
		return;	//One-component image

	const unsigned short bpp = GetBPP();
	const size_t sizeInBytes = GetSize();

	for (unsigned int i = 0; i < sizeInBytes; i += bpp) {
		unsigned char tmp = m_Data[i];
		m_Data[i] = m_Data[i + 2];
		m_Data[i + 2] = tmp;
	}
}


void CImage::FlipVertical()
{
	assert(m_Data != NULL);

	const unsigned int sizeWidth = m_ImgWidth * GetBPP();
	unsigned char* swapBuffer = new unsigned char[sizeWidth];

	for (unsigned int i = 0; i < m_ImgHeight / 2; ++i) {
		unsigned char* srcData = m_Data + i * sizeWidth;
		unsigned char* dstData = m_Data + (m_ImgHeight - i - 1) * sizeWidth;

		memcpy(swapBuffer, dstData, sizeWidth);
		memcpy(dstData, srcData, sizeWidth);
		memcpy(srcData, swapBuffer, sizeWidth);
	}

	delete[] swapBuffer;
}
