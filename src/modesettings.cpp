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

#include "modesettings.h"
#include "texture.h"
#include "settings.h"
#include "game.h"
#include "rendertext.h"


void CModeSettings::Initialize()
{
	const float btnSize = 0.5f;
	const float btnLeft = -0.25f;

	_MapSize.AddButton(CCheckBoxButton(_Game, false, btnLeft, 3.8f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeSmall));
	_MapSize.AddButton(CCheckBoxButton(_Game, false, btnLeft, 3.1f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeNormal));
	_MapSize.AddButton(CCheckBoxButton(_Game, false, btnLeft, 2.4f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeBig));
	_MapSize.AddButton(CCheckBoxButton(_Game, false, btnLeft, 1.7f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, MapSizeExtra));

	_WrapMode.AddButton(CCheckBoxButton(_Game, false, btnLeft, 0.6f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 1));
	_WrapMode.AddButton(CCheckBoxButton(_Game, false, btnLeft, -0.1f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 0));

	_Sound.AddButton(CCheckBoxButton(_Game, false, btnLeft, -1.2f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 1));
	_Sound.AddButton(CCheckBoxButton(_Game, false, btnLeft, -1.9f, btnSize, btnSize, CTextureBank::TexRadBtnOn, CTextureBank::TexRadBtnOff, 0));
	
	_PrevTheme.Init(btnLeft, -3.0f, btnSize, btnSize, CTextureBank::TexButtonPrev, 0);
	_NextTheme.Init(4.1f, -3.0f, btnSize, btnSize, CTextureBank::TexButtonNext, 0);

	Reset();
}


void CModeSettings::Reset()
{
	_MapSize.SetChoice(_Game.UserSettings().Size);
	_WrapMode.SetChoice(_Game.UserSettings().Wrapping ? 1 : 0);
	_Sound.SetChoice(_Game.UserSettings().Sound ? 1 : 0);
}


void CModeSettings::Render(const float mouseX, const float mouseY, const float transition)
{
	static const float titleSize = 0.5f;
	static const float choiceSize = 0.4f;

	const CGameSettings::Theme& theme = _Game.GameSettings().Themes[_Game.UserSettings().ThemeId];

	float colorTitle[4] = { theme.TitleColor[0], theme.TitleColor[1], theme.TitleColor[2], transition };
	float colorText[4] = { theme.TextColor[0], theme.TextColor[1], theme.TextColor[2], transition };
	
	const CRenderText& tr = _Game.RenderText();
	tr.Print(-4.0f, 4.8f, 0.6f, colorTitle, true, "O P T I O N S");

	tr.Print(-2.4f, 3.8f, titleSize, colorTitle, true, "Map:");
	tr.Print(0.3f, 3.75f, choiceSize, colorText, true, "Small");
	tr.Print(0.3f, 3.05f, choiceSize, colorText, true, "Normal");
	tr.Print(0.3f, 2.35f, choiceSize, colorText, true, "Big");
	tr.Print(0.3f, 1.65f, choiceSize, colorText, true, "Extra");

 	tr.Print(-2.9f, 0.6f, titleSize, colorTitle, true, "Wrap:");
	tr.Print(0.3f, 0.55f, choiceSize, colorText, true, "On");
	tr.Print(0.3f, -0.15f, choiceSize, colorText, true, "Off");

	tr.Print(-3.4f, -1.2f, titleSize, colorTitle, true, "Sound:");
	tr.Print(0.3f, -1.25f, choiceSize, colorText, true, "On");
	tr.Print(0.3f, -1.95f, choiceSize, colorText, true, "Off");

	tr.Print(-3.4f, -2.9f, titleSize, colorTitle, true, "Theme:");
	tr.Print(0.3f, -3.05f, choiceSize, colorText, true, theme.Name.c_str());

	tr.Print(-4.9f, -4.2f, 0.33f, colorTitle, true, "& 2007-2010 Artem A. Senichev");
	tr.Print(-1.8f, -4.6f, 0.28f, colorTitle, true, "Moscow, Russia");

	//Draw buttons
	glColor4f(1.0f, 1.0, 1.0f, transition);
	_MapSize.Render(mouseX, mouseY);
	_WrapMode.Render(mouseX, mouseY);
	_Sound.Render(mouseX, mouseY);
	_PrevTheme.Render(mouseX, mouseY);
	_NextTheme.Render(mouseX, mouseY);
	glColor4f(1.0f, 1.0, 1.0f, 1.0f);
}


void CModeSettings::OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn)
{
	if (btn == MouseButton_Left) {
		bool redrawNeeded = false;

		const bool nextTheme = _NextTheme.IsMouseOver(mouseX, mouseY);
		const bool prevTheme = _PrevTheme.IsMouseOver(mouseX, mouseY);
		if (nextTheme || prevTheme) {
			size_t& themeId = _Game.UserSettings().ThemeId;

			if (nextTheme) {
				if (++themeId >= _Game.GameSettings().Themes.size())
					themeId = 0;
			}
			else {
				if (themeId != 0)
					--themeId;
				else
					themeId = _Game.GameSettings().Themes.size() - 1;
			}
			string textFileName = DIR_GAMEDATA;
			textFileName += _Game.GameSettings().Themes[themeId].TextureFile;
			_Game.ReloadTextures();
			redrawNeeded = true;
		}
		else {
			redrawNeeded |= _MapSize.OnClick(mouseX, mouseY);
			redrawNeeded |= _WrapMode.OnClick(mouseX, mouseY);
			redrawNeeded |= _Sound.OnClick(mouseX, mouseY);
		}
		
		if (redrawNeeded)
			_Game.WinManager().PostRedisplay();
	}
}
