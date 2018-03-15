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

#include "modesettings.h"
#include "texture.h"
#include "settings.h"


void CModeSettings::Initialize()
{
	const GLuint texOn = CTextureBank::Get(CTextureBank::TexRadBtnOn);
	const GLuint texOff = CTextureBank::Get(CTextureBank::TexRadBtnOff);
	const float btnSize = 0.6f;
	const float btnLeft = -1.6f;

	m_MapChoose.reserve(3);
	m_MapChoose.push_back(CRadioButton(false, btnLeft,  1.10f, btnSize, btnSize, texOn, texOff, MapSizeNormal));
	m_MapChoose.push_back(CRadioButton(false, btnLeft,  0.35f, btnSize, btnSize, texOn, texOff, MapSizeBig));
	m_MapChoose.push_back(CRadioButton(false, btnLeft, -0.40f, btnSize, btnSize, texOn, texOff, MapSizeParanoid));
	SetMapSize(CSettings::Settings.Size);
}


bool CModeSettings::Render(const float mouseX, const float mouseY, const float transition)
{
	bool redarawIsNeeded = false;
	glColor4f(1.0f, 1.0, 1.0f, transition);

	//Draw background
	static const float vertexBkgr[] =	{ -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
	static const short texBkgr[] =		{ 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned int indicesBkgr[] =	{ 0, 1, 2, 0, 2, 3 };
	glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvSett));
	glVertexPointer(2, GL_FLOAT, 0, vertexBkgr);
	glTexCoordPointer(2, GL_SHORT, 0, texBkgr);
	glDrawElements(GL_TRIANGLES, (sizeof(indicesBkgr) / sizeof(indicesBkgr[0])), GL_UNSIGNED_INT, indicesBkgr);

	//Radio buttons
	for (vector<CRadioButton>::const_iterator itBtn = m_MapChoose.begin(); itBtn != m_MapChoose.end(); ++itBtn)
		itBtn->Render(mouseX, mouseY);

	glColor4f(1.0f, 1.0, 1.0f, 1.0f);
	return redarawIsNeeded;
}


void CModeSettings::OnMouseClick(const Uint8 button, const float mouseX, const float mouseY)
{
	if (button != SDL_BUTTON_LEFT)
		return;

	const MapSize curr = GetMapSize();
	for (vector<CRadioButton>::iterator itBtn = m_MapChoose.begin(); itBtn != m_MapChoose.end(); ++itBtn)
		itBtn->SetState(false);

	bool found = false;
	for (vector<CRadioButton>::iterator itBtn = m_MapChoose.begin(); itBtn != m_MapChoose.end(); ++itBtn) {
		if (itBtn->IsMouseOver(mouseX, mouseY)) {
			found = true;
			itBtn->SetState(true);
		}
	}

	if (!found)
		SetMapSize(curr);
}


MapSize CModeSettings::GetMapSize() const
{
	for (vector<CRadioButton>::const_iterator itBtn = m_MapChoose.begin(); itBtn != m_MapChoose.end(); ++itBtn) {
		if (itBtn->GetState())
			return static_cast<MapSize>(itBtn->GetId());
	}
	assert(false && "Undefined state");
	return MapSizeNormal;
}


void CModeSettings::SetMapSize(const MapSize mapSize)
{
	for (vector<CRadioButton>::iterator itBtn = m_MapChoose.begin(); itBtn != m_MapChoose.end(); ++itBtn)
		itBtn->SetState(static_cast<MapSize>(itBtn->GetId()) == mapSize);
}
