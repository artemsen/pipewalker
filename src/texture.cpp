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

#include "texture.h"
#include "image.h"


void CTexture::Free()
{
	if (_Id)
		glDeleteTextures(1, &_Id);
	_Id = 0;
}


void CTexture::Load(const CImage& img, const size_t x, const size_t y, const size_t width, const size_t height, const int modeWrap /*= GL_CLAMP*/)
{
	assert(!(width & width - 1));	//Pow of 2
	assert(!(height & height - 1));	//Pow of 2

	vector<unsigned char> imgData;
	img.GetSubImage(x, y, width, height, imgData);

	glGenTextures(1, &_Id);

	glBindTexture(GL_TEXTURE_2D, _Id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeWrap);
	glTexImage2D(GL_TEXTURE_2D, 0, img.GetMode(), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, img.GetMode(), GL_UNSIGNED_BYTE, &imgData[0]);
}


void CTexture::Load(const char* fileName, const int modeWrap /*= GL_CLAMP*/)
{
	assert(fileName);

	Free();

	CImage img;
	img.Load(fileName);

	assert(!(img.GetWidth() & img.GetWidth() - 1));		//Pow of 2
	assert(!(img.GetHeight() & img.GetHeight() - 1));	//Pow of 2

	glGenTextures(1, &_Id);

	glBindTexture(GL_TEXTURE_2D, _Id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeWrap);
	glTexImage2D(GL_TEXTURE_2D, 0, img.GetMode(), static_cast<GLsizei>(img.GetWidth()), static_cast<GLsizei>(img.GetHeight()), 0, img.GetMode(), GL_UNSIGNED_BYTE, img.GetData());
}


CTextureBank::~CTextureBank()
{
	Free();
}


void CTextureBank::Load(const char* fileName)
{
	assert(fileName);

	Free();

	CImage img;
	img.Load(fileName);

	_Texture[TexEnvBkgr].        Load(img,  8 * 64, 0 * 64, 64, 64, GL_REPEAT);
	_Texture[TexEnvTitle].       Load(img,  0 * 64, 0 * 64, 8 * 64, 64, GL_CLAMP);
	_Texture[TexCellBackground]. Load(img,  9 * 64, 0 * 64, 64, 64, GL_REPEAT);
	_Texture[TexSender].         Load(img,  0 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexReceiverPassive].Load(img,  1 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexReceiverActive]. Load(img,  2 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexLock].           Load(img,  3 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeHalfPassive].Load(img,  4 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeHalfActive]. Load(img,  5 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeStrPassive]. Load(img,  6 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeStrActive].  Load(img,  7 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeCrvPassive]. Load(img,  8 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeCrvActive].  Load(img,  9 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeJnrPassive]. Load(img, 10 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexTubeJnrActive].  Load(img, 11 * 64, 2 * 64, 64, 64, GL_CLAMP);
	_Texture[TexExplosionPart].  Load(img,  6 * 64, 1 * 64 + 32, 32, 32, GL_CLAMP);
	_Texture[TexNum0].	         Load(img,  7 * 64, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum1].	         Load(img,  7 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum2].	         Load(img,  8 * 64, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum3].	         Load(img,  8 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum4].	         Load(img,  9 * 64, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum5].	         Load(img,  9 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum6].	         Load(img, 10 * 64, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum7].	         Load(img, 10 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum8].	         Load(img, 11 * 64, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexNum9].	         Load(img, 11 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	_Texture[TexButtonPrev].     Load(img,  0 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexButtonNext].     Load(img,  1 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexButtonOK].       Load(img,  2 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexButtonCancel].   Load(img,  3 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexButtonReset].    Load(img,  4 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexButtonSett].     Load(img,  5 * 64, 1 * 64, 64, 64, GL_CLAMP);
	_Texture[TexRadBtnOff].      Load(img, 10 * 64, 0 * 64, 64, 64, GL_CLAMP);
	_Texture[TexRadBtnOn].       Load(img, 11 * 64, 0 * 64, 64, 64, GL_CLAMP);
}


GLuint CTextureBank::Get(const TextureType type) const
{
	assert(type >= 0 && type < TexCounter);
	return _Texture[type].GetId();
}


void CTextureBank::Free()
{
	for (size_t i = 0; i < TexCounter; ++i)
		_Texture[i].Free();
}
