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
	const GLuint texOn = CTextureBank::Get(CTextureBank::TexRadBtnOn);
	const GLuint texOff = CTextureBank::Get(CTextureBank::TexRadBtnOff);
	const float btnSize = 0.6f;
	const float btnLeft = -0.5f;

	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 4.0f, btnSize, btnSize, texOn, texOff, MapSizeSmall));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 3.3f, btnSize, btnSize, texOn, texOff, MapSizeNormal));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 2.6f, btnSize, btnSize, texOn, texOff, MapSizeBig));
	m_MapSize.AddButton(CCheckBoxButton(false, btnLeft, 1.9f, btnSize, btnSize, texOn, texOff, MapSizeExtra));

	m_WrapMode.AddButton(CCheckBoxButton(false, btnLeft, 0.8f, btnSize, btnSize, texOn, texOff, 1));
	m_WrapMode.AddButton(CCheckBoxButton(false, btnLeft, 0.1f, btnSize, btnSize, texOn, texOff, 0));

	m_Sound.AddButton(CCheckBoxButton(false, btnLeft, -1.0f, btnSize, btnSize, texOn, texOff, 1));
	m_Sound.AddButton(CCheckBoxButton(false, btnLeft, -1.7f, btnSize, btnSize, texOn, texOff, 0));
	
	m_PrevTheme.Init(btnLeft, -2.8f, btnSize, btnSize, CTextureBank::Get(CTextureBank::TexButtonPrev), 0);
	m_NextTheme.Init(4.0f, -2.8f, btnSize, btnSize, CTextureBank::Get(CTextureBank::TexButtonNext), 0);

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
	//Draw text
	const float colorText[4] = { 0.0f, 0.5f, 1.0f, transition };
	const float colorTitle[4] = { 0.0f, 0.3f, 1.0f, transition };

	CRenderText::Print(-4.4f, 4.0f, 0.7f, colorTitle, true, "Map size:");
	CRenderText::Print(0.2f, 4.0f, 0.6f, colorText, true, "Small  [10x10]");
	CRenderText::Print(0.2f, 3.3f, 0.6f, colorText, true, "Normal [14x14]");
	CRenderText::Print(0.2f, 2.6f, 0.6f, colorText, true, "Big    [20x20]");
	CRenderText::Print(0.2f, 1.9f, 0.6f, colorText, true, "Extra  [30x30]");

 	CRenderText::Print(-4.8f, 0.8f, 0.7f, colorTitle, true, "Wrap mode:");
	CRenderText::Print(0.2f, 0.8f, 0.6f, colorText, true, "On");
	CRenderText::Print(0.2f, 0.1f, 0.6f, colorText, true, "Off");

	CRenderText::Print(-3.2f, -1.0f, 0.7f, colorTitle, true, "Sound:");
	CRenderText::Print(0.2f, -1.0f, 0.6f, colorText, true, "On");
	CRenderText::Print(0.2f, -1.7f, 0.6f, colorText, true, "Off");

	CRenderText::Print(-3.2f, -2.7f, 0.7f, colorTitle, true, "Theme:");
	const char* themeName = (m_Game.Settings().Theme == ThemeNetwork ? "Network" : "Plumbing");
	CRenderText::Print(0.8f, -2.8f, 0.6f, colorText, true, themeName);

	const float colorInfo[4] = { 0.0f, 0.3f, 0.8f, transition };
	CRenderText::Print(-4.8f, -4.2f, 0.52f, colorInfo, true, "http://pipewalker.sourceforge.net");
	CRenderText::Print(-4.78f, -4.7f, 0.3f, colorInfo, true, "Copyright (C) 2007-2010 Artem A. Senichev, Moscow, Russia");

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
