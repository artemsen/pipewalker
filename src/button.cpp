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

#include "button.h"
#include "game.h"


CButton::CButton(CGame& game)
:	_Game(game),
	_X(0.0f),
	_Y(0.0f),
	_Width(0.0f),
	_Height(0.0f),
	_BtnId(0),
	_TexId(CTextureBank::TexCounter)
{
}


CButton::CButton(CGame& game, const float x, const float y, const float width, const float height, const CTextureBank::TextureType tex, const int id)
:	_Game(game)
{
	Init(x, y, width, height, tex, id);
}


void CButton::Init(const float x, const float y, const float width, const float height, const CTextureBank::TextureType tex, const int id)
{
	_X = x;
	_Y = y;
	_Width = width;
	_Height = height;
	_TexId = tex;
	_BtnId = id;
}


bool CButton::IsMouseOver(const float x, const float y) const
{
	return  x >= _X && x <= _X + _Width &&
			y <= _Y && y >= _Y - _Height;
}


void CButton::Render(const float x, const float y) const
{
	RenderButton(x, y, _TexId);
}


void CButton::RenderButton(const float x, const float y, const CTextureBank::TextureType texture) const
{
	const float coeff = IsMouseOver(x, y) ? 0.05f : 0.0f;

	//Button vertex coordinates
	const float Vertex[] = {
		_X - coeff, _Y + coeff,
		_X - coeff, _Y - _Height - coeff,
		_X + _Width + coeff, _Y - _Height - coeff,
		_X + _Width + coeff, _Y + coeff };

	static const short Tex[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };	//Texture coordinates
	static const unsigned short Ind[] =	{ 0, 1, 2, 0, 2, 3 };		//Indices

	glBindTexture(GL_TEXTURE_2D, _Game.TextureBank().Get(texture));
	glVertexPointer(2, GL_FLOAT, 0, Vertex);
	glTexCoordPointer(2, GL_SHORT, 0, Tex);
	glDrawElements(GL_TRIANGLES, (sizeof(Ind) / sizeof(Ind[0])), GL_UNSIGNED_SHORT, Ind);
}


CCheckBoxButton::CCheckBoxButton(CGame& game, const bool state, const float x, const float y, const float width, const float height, const CTextureBank::TextureType texOn, const CTextureBank::TextureType texOff, const int id)
: CButton(game, x, y, width, height, texOn, id), _State(state), _TexOff(texOff)
{
}


void CCheckBoxButton::Render(const float x, const float y) const
{
	RenderButton(x, y, _State ? _TexId : _TexOff);
}


void CRadioButtons::AddButton(const CCheckBoxButton& btn)
{
	_Buttons.push_back(btn);
}


void CRadioButtons::Render(const float mouseX, const float mouseY) const
{
	for (list<CCheckBoxButton>::const_iterator itBtn = _Buttons.begin(); itBtn != _Buttons.end(); ++itBtn)
		itBtn->Render(mouseX, mouseY);
}


bool CRadioButtons::OnClick(const float mouseX, const float mouseY)
{
	bool stateChanged = false;
	for (list<CCheckBoxButton>::const_iterator itBtn = _Buttons.begin(); itBtn != _Buttons.end() && !stateChanged; ++itBtn)
		stateChanged = itBtn->IsMouseOver(mouseX, mouseY) && !itBtn->GetState();

	if (stateChanged) {
		for (list<CCheckBoxButton>::iterator itBtn = _Buttons.begin(); itBtn != _Buttons.end(); ++itBtn)
			itBtn->SetState(itBtn->IsMouseOver(mouseX, mouseY));
	}
	return stateChanged;
}


int CRadioButtons::GetChoice() const
{
	int id = -1;
	for (list<CCheckBoxButton>::const_iterator itBtn = _Buttons.begin(); itBtn != _Buttons.end() && id < 0; ++itBtn)
		if (itBtn->GetState())
			id = itBtn->GetId();
	return id;
}


void CRadioButtons::SetChoice(const int choiceId)
{
	for (list<CCheckBoxButton>::iterator itBtn = _Buttons.begin(); itBtn != _Buttons.end(); ++itBtn)
		itBtn->SetState(itBtn->GetId() == choiceId);
}
