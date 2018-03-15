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

#include "texture.h"
#include "image.h"

CTexture CTextureBank::m_Texture[CTextureBank::TexCounter];


void CTexture::Free()
{
	if (m_Id)
		glDeleteTextures(1, &m_Id);
	m_Id = 0;
}


void CTexture::Load(const CImage& img, const size_t x, const size_t y, const size_t width, const size_t height, const int modeWrap /*= GL_CLAMP*/)
{
	assert(!(width & width - 1));	//Pow of 2
	assert(!(height & height - 1));	//Pow of 2

	vector<unsigned char> imgData;
	img.GetSubImage(x, y, width, height, imgData);

	glGenTextures(1, &m_Id);

	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeWrap);
	glTexImage2D(GL_TEXTURE_2D, 0, img.GetMode(), width, height, 0, img.GetMode(), GL_UNSIGNED_BYTE, &imgData[0]);
}


void CTexture::Load(const char* fileName, const int modeWrap /*= GL_CLAMP*/)
{
	assert(fileName);

	Free();

	CImage img;
	img.Load(fileName);

	assert(!(img.GetWidth() & img.GetWidth() - 1));		//Pow of 2
	assert(!(img.GetHeight() & img.GetHeight() - 1));	//Pow of 2

	glGenTextures(1, &m_Id);

	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeWrap);
	glTexImage2D(GL_TEXTURE_2D, 0, img.GetMode(), img.GetWidth(), img.GetHeight(), 0, img.GetMode(), GL_UNSIGNED_BYTE, img.GetData());
}


void CTextureBank::Load()
{
	Free();

	CImage img;
	img.Load(DIR_GAMEDATA "network.tga");

	m_Texture[TexEnvBkgr].          Load(img,  8 * 64, 0 * 64, 64, 64, GL_REPEAT);
	m_Texture[TexEnvTitle].         Load(img,  0 * 64, 0 * 64, 8 * 64, 64, GL_CLAMP);
	m_Texture[TexCellBackground].   Load(img,  9 * 64, 0 * 64, 64, 64, GL_REPEAT);
	m_Texture[TexSender].           Load(img,  0 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexReceiverPassive].  Load(img,  1 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexReceiverActive].   Load(img,  2 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexLock].             Load(img,  3 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeHalfPassive].  Load(img,  4 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeHalfActive].   Load(img,  5 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeStrPassive].   Load(img,  6 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeStrActive].    Load(img,  7 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeCrvPassive].   Load(img,  8 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeCrvActive].    Load(img,  9 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeJnrPassive].   Load(img, 10 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexTubeJnrActive].    Load(img, 11 * 64, 2 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexExplosionPart].    Load(img,  6 * 64, 1 * 64 + 32, 32, 32, GL_CLAMP);
	m_Texture[TexNum0].	            Load(img,  7 * 64, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum1].	            Load(img,  7 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum2].	            Load(img,  8 * 64, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum3].	            Load(img,  8 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum4].	            Load(img,  9 * 64, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum5].	            Load(img,  9 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum6].	            Load(img, 10 * 64, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum7].	            Load(img, 10 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum8].	            Load(img, 11 * 64, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexNum9].	            Load(img, 11 * 64 + 32, 1 * 64, 32, 64, GL_CLAMP);
	m_Texture[TexButtonPrev].       Load(img,  0 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexButtonNext].       Load(img,  1 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexButtonOK].         Load(img,  2 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexButtonCancel].     Load(img,  3 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexButtonReset].      Load(img,  4 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexButtonSett].       Load(img,  5 * 64, 1 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexRadBtnOff].        Load(img, 10 * 64, 0 * 64, 64, 64, GL_CLAMP);
	m_Texture[TexRadBtnOn].         Load(img, 11 * 64, 0 * 64, 64, 64, GL_CLAMP);
}


GLuint CTextureBank::Get(const TextureType type)
{
	assert(type >= 0 && type < TexCounter);
	return m_Texture[type].GetId();
}


void CTextureBank::Free()
{
	for (size_t i = 0; i < TexCounter; ++i)
		m_Texture[i].Free();
}
