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

#include "image.h"
#include "buffer.h"

#pragma pack (push, 1)
//! Targa header
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
#pragma pack (pop)


CImage::CImage()
:	m_ImgWidth(0),
	m_ImgHeight(0),
	m_ImgMode(GL_RGB)
{
}


void CImage::Load(const char* fileName)
{
	assert(fileName);

	CBuffer buf;
	buf.Load(fileName);

	const IMAGE_TGA_HEADER* header = reinterpret_cast<const IMAGE_TGA_HEADER*>(buf.GetData(sizeof(IMAGE_TGA_HEADER)));
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
	m_Data.resize(sizeInBytes);
	if (header->ImageType == IMAGE_TGA_RGB)
		memcpy(&m_Data[0], buf.GetData(sizeInBytes), sizeInBytes);
	else {
		//Create new own buffer to store image data
		size_t bufPos = 0;

		while (bufPos < sizeInBytes) {
			unsigned char packetHeader;
			if (!buf.Get(packetHeader))
				throw CException("TGA: Incorrect format (not enough data)");
			const unsigned char packetLength = (packetHeader & 0x7F) + 1;
			if ((packetHeader & 0x80) != 0) {	// Run-length encoding packet (RLE)
				const unsigned char* packet = reinterpret_cast<const unsigned char*>(buf.GetData(bpp));
				for (unsigned int j = 0; j < packetLength * bpp; ++j) {
					if (bufPos >= sizeInBytes)
						throw CException("TGA: Incorrect format");
					m_Data[bufPos++] = packet[j % bpp];
				}
			}
			else {								//Raw packet
				for (size_t j = 0; j < packetLength * bpp; ++j) {
					if (bufPos >= sizeInBytes || !buf.Get(m_Data[bufPos++]))
						throw CException("TGA: Incorrect format");
				}
			}
		}
	}

	ConvertBGRtoRGB();

	// Check flip bit
	if ((header->ImageDesc & 0x20) != 0)
		FlipVertical();
}


void CImage::LoadXPM(char* data[], const size_t strNum)
{
	assert(data);

	m_ImgMode = GL_RGBA;

	//Read image properties
	int colorMapSize = 0, bytePerPixel = 0, imgWidth = 0, imgHeight = 0;
	sscanf(data[0], "%i %i %i %i", &imgWidth, &imgHeight, &colorMapSize, &bytePerPixel);
	m_ImgWidth = static_cast<size_t>(imgWidth);
	m_ImgHeight = static_cast<size_t>(imgHeight);

	//Read the color map
	map< string, vector<unsigned char> > colorMap;
	for (int i = 1; i <= colorMapSize; ++i) {
		if (i >= static_cast<int>(strNum))
			throw CException("XPM: Incorrect format");

		const string index(data[i], data[i] + bytePerPixel);
		vector<unsigned char> color;
		color.resize(4, 0);
		if (strcmp(data[i] + bytePerPixel + 3, "None") != 0) {
			unsigned int colorRef = 0;
			sscanf(data[i] + bytePerPixel + 4, "%x", &colorRef);
#ifdef PW_BYTEORDER_LITTLE_ENDIAN
			color[0] = static_cast<unsigned char>((colorRef & 0x00ff0000) >> 16);
			color[1] = static_cast<unsigned char>((colorRef & 0x0000ff00) >> 8);
			color[2] = static_cast<unsigned char>((colorRef & 0x000000ff) >> 0);
#else
			color[0] = static_cast<unsigned char>((colorRef & 0xff000000) << 0);
			color[1] = static_cast<unsigned char>((colorRef & 0x00ff0000) << 8);
			color[2] = static_cast<unsigned char>((colorRef & 0x0000ff00) << 16);
#endif	//PW_BYTEORDER_LITTLE_ENDIAN
			color[3] = 0xff;	//Non transparent
		}
		colorMap.insert(make_pair(index, color));
	}

	//Read image data
	m_Data.reserve(SizeInBytes());
	for (size_t i = 0; i < m_ImgHeight; ++i) {
		if (i + colorMapSize + 1 >= strNum)
			throw CException("XPM: Incorrect format");
		const char* imgData = data[i + colorMapSize + 1];
		for (size_t j = 0; j < m_ImgWidth; ++j) {
			const string colorIndex(imgData + j * bytePerPixel, imgData + j * bytePerPixel + bytePerPixel);
			const vector<unsigned char> colorValue = colorMap[colorIndex];
			assert(colorValue.size() == 4);
			copy(colorValue.begin(), colorValue.end(), back_inserter(m_Data));
		}
	}
	assert(m_Data.size() == SizeInBytes());
}


void CImage::GetSubImage(const size_t x, const size_t y, const size_t width, const size_t height, vector<unsigned char>& data) const
{
	assert(!m_Data.empty());
	assert(m_ImgMode == GL_RGB || m_ImgMode == GL_RGBA);

	if (x > m_ImgWidth || x + width > m_ImgWidth || y > m_ImgHeight || y + height > m_ImgHeight)
		throw CException("Sub image coordinates out of range");

	const unsigned char bpp = BytesPerPixel();

	data.reserve(height * width * bpp);
	for (size_t i = 0; i < height; ++i) {
		const vector<unsigned char>::const_iterator itCopy = m_Data.begin() + x * bpp + (i + y) * m_ImgWidth * bpp;
		copy(itCopy, itCopy + width * bpp, back_inserter(data));
	}
}


void CImage::ConvertBGRtoRGB()
{
	assert(!m_Data.empty());
	assert(m_ImgMode == GL_RGB || m_ImgMode == GL_RGBA);

	if (m_ImgMode != GL_RGB && m_ImgMode != GL_RGBA)
		return;	//One-component image

	const unsigned short bpp = BytesPerPixel();
	const size_t sizeInBytes = SizeInBytes();

	for (unsigned int i = 0; i < sizeInBytes; i += bpp) {
		unsigned char tmp = m_Data[i];
		m_Data[i] = m_Data[i + 2];
		m_Data[i + 2] = tmp;
	}
}


void CImage::FlipVertical()
{
	assert(!m_Data.empty());

	const unsigned int sizeWidth = m_ImgWidth * BytesPerPixel();
	vector<unsigned char> swapBuffer;
	swapBuffer.resize(sizeWidth);

	for (unsigned int i = 0; i < m_ImgHeight / 2; ++i) {
		unsigned char* srcData = &m_Data[i * sizeWidth];
		unsigned char* dstData = &m_Data[(m_ImgHeight - i - 1) * sizeWidth];

		memcpy(&swapBuffer[0], dstData, sizeWidth);
		memcpy(dstData, srcData, sizeWidth);
		memcpy(srcData, &swapBuffer[0], sizeWidth);
	}
}
