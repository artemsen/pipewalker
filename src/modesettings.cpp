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
#include "game.h"
#include "rendertext.h"


void CModeSettings::Initialize()
{
	const float btnSize = 0.5f;
	const float btnLeft = -0.25f;

	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 3.8f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeSmall));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 3.1f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeNormal));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 2.4f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeBig));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 1.7f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeExtra));

	m_WrapMode.AddButton(CCheckBoxButton(false, btnLeft, 0.6f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 1));
	m_WrapMode.AddButton(CCheckBoxButton(false, btnLeft, -0.1f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 0));

	m_Sound.AddButton(CCheckBoxButton(false, btnLeft, -1.2f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 1));
	m_Sound.AddButton(CCheckBoxButton(false, btnLeft, -1.9f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 0));
	
	m_PrevTheme.Init(btnLeft, -3.0f, btnSize, btnSize, CTextureBank::TexButtonPrev, 0);
	m_NextTheme.Init(4.0f, -3.0f, btnSize, btnSize, CTextureBank::TexButtonNext, 0);

	Reset();
}


void CModeSettings::Reset()
{
	m_MapSize.SetChoice(m_Game.Settings().Size);
	m_WrapMode.SetChoice(m_Game.Settings().Wrapping ? 1 : 0);
	m_Sound.SetChoice(m_Game.Settings().Sound ? 1 : 0);
}


void CModeSettings::Render(const float mouseX, const float mouseY, const float transition)
{
	const float titleSize = 0.5f;
	const float choiceSize = 0.4f;

	float colorText[4] = { 0.0f, 0.0f, 0.0f, transition };
	float colorTitle[4] = { 0.0f, 0.0f, 0.0f, transition };

	switch (m_Game.Settings().Theme) {
		case ThemePlumbing: 
			colorText[0] = 0.3f;
			colorText[1] = 0.4f;
			colorText[2] = 0.5f;
			colorTitle[0] = 0.2f;
			colorTitle[1] = 0.3f;
			colorTitle[2] = 0.4f;
			break;
		case ThemeNetwork: 
		default:
			colorText[1] = 0.6f;
			colorTitle[1] = 0.5f;
			colorText[2] = 0.9f;
			colorTitle[2] = 0.8f;
	}

	CRenderText::Print(-4.0f, 4.8f, 0.6f, colorTitle, true, "O P T I O N S");

	CRenderText::Print(-2.4f, 3.8f, titleSize, colorTitle, true, "Map:");
	CRenderText::Print(0.3f, 3.75f, choiceSize, colorText, true, "Small");
	CRenderText::Print(0.3f, 3.05f, choiceSize, colorText, true, "Normal");
	CRenderText::Print(0.3f, 2.35f, choiceSize, colorText, true, "Big");
	CRenderText::Print(0.3f, 1.65f, choiceSize, colorText, true, "Extra");

 	CRenderText::Print(-2.9f, 0.6f, titleSize, colorTitle, true, "Wrap:");
	CRenderText::Print(0.3f, 0.55f, choiceSize, colorText, true, "On");
	CRenderText::Print(0.3f, -0.15f, choiceSize, colorText, true, "Off");

	CRenderText::Print(-3.4f, -1.2f, titleSize, colorTitle, true, "Sound:");
	CRenderText::Print(0.3f, -1.25f, choiceSize, colorText, true, "On");
	CRenderText::Print(0.3f, -1.95f, choiceSize, colorText, true, "Off");

	CRenderText::Print(-3.4f, -2.9f, titleSize, colorTitle, true, "Theme:");
	const char* themeName = (m_Game.Settings().Theme == ThemeNetwork ? "Network" : "Plumbing");
	CRenderText::Print(0.3f, -3.05f, choiceSize, colorText, true, themeName);

	CRenderText::Print(-4.9f, -4.2f, 0.33f, colorTitle, true, "& 2007-2010 Artem A. Senichev");
	CRenderText::Print(-1.8f, -4.6f, 0.28f, colorTitle, true, "Moscow, Russia");

	//Draw buttons
	glColor4f(1.0f, 1.0, 1.0f, transition);
	m_MapSize.Render(mouseX, mouseY);
	m_WrapMode.Render(mouseX, mouseY);
	m_Sound.Render(mouseX, mouseY);
	m_PrevTheme.Render(mouseX, mouseY);
	m_NextTheme.Render(mouseX, mouseY);
	glColor4f(1.0f, 1.0, 1.0f, 1.0f);
}


void CModeSettings::OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn)
{
	if (btn == MouseButton_Left) {

		if (m_NextTheme.IsMouseOver(mouseX, mouseY) || m_PrevTheme.IsMouseOver(mouseX, mouseY)) {
			if (m_Game.Settings().Theme == ThemeNetwork)
				m_Game.Settings().Theme = ThemePlumbing;
			else
				m_Game.Settings().Theme = ThemeNetwork;
			CTextureBank::Load(m_Game.Settings().Theme);
			m_Game.WinManager().PostRedisplay();
		}
		else {
			bool redrawNeeded = false;
			redrawNeeded |= m_MapSize.OnClick(mouseX, mouseY);
			redrawNeeded |= m_WrapMode.OnClick(mouseX, mouseY);
			redrawNeeded |= m_Sound.OnClick(mouseX, mouseY);
			if (redrawNeeded)
				m_Game.WinManager().PostRedisplay();
		}
	}
}
