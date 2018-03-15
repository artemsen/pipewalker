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

#include "modepuzzle.h"
#include "sound.h"
#include "settings.h"
#include "game.h"


void CModePuzzle::RenewMap()
{
	StopWinnerAnimation();
	_UseSound = false;
	const CUserSettings& settings = _Game.UserSettings();
	_Map.New(settings.Size, settings.GetCurrentMapId(), settings.Wrapping);
}


void CModePuzzle::LoadMap()
{
	StopWinnerAnimation();
	const CUserSettings& settings = _Game.UserSettings();
	if (!_Map.LoadMap(settings.Size, settings.GetCurrentMapState(), settings.Wrapping))
		RenewMap();
}


void CModePuzzle::SaveMap() const
{
	CUserSettings& settings = _Game.UserSettings();
	settings.Size = _Map.GetMapSize();
	settings.SetCurrentMapState(_Map.SaveMap());
}


void CModePuzzle::Render(const float transition)
{
	bool redarawIsNeeded = false;

	redarawIsNeeded |= RenderPuzzle(transition);

	//Render map ID
	static const float mapIdVertex[] =				{ -0.2f, 0.4f, -0.2f, -0.4f, 0.2f, -0.4f, 0.2f, 0.4f };
	static const short mapIdTexture[] =				{ 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned short mapIdIndices[] =	{ 0, 1, 2, 0, 2, 3 };
	glColor4f(1.0f, 1.0, 1.0f, transition);
	char szMapId[64];
	sprintf(szMapId, "%08u", static_cast<unsigned int>(_Game.UserSettings().GetCurrentMapId()));
	string mapIdAsString(szMapId);
	glVertexPointer(2, GL_FLOAT, 0, mapIdVertex);
	glTexCoordPointer(2, GL_SHORT, 0, mapIdTexture);
	glPushMatrix();
		glTranslatef(-2.25f, -5.6f, 0.0f);
		for (size_t i = 0; i < 8; ++i) {
			glBindTexture(GL_TEXTURE_2D, _Game.TextureBank().Get(static_cast<CTextureBank::TextureType>(CTextureBank::TexNum0 + (mapIdAsString[i] - '0'))));
			glTranslatef(0.5f, 0.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, (sizeof(mapIdIndices) / sizeof(mapIdIndices[0])), GL_UNSIGNED_SHORT, mapIdIndices);
		}
		glColor4f(1.0f, 1.0, 1.0f, 1.0f);
	glPopMatrix();

	//Render winner explosions
	if (_Map.IsGameOver()) {
		const unsigned short mapSize = static_cast<const unsigned short>(_Map.GetMapSize());

		if (_Explosions.empty()) {
			//Create explosions
			for (unsigned short y = 0; y < mapSize; ++y) {
				const GLfloat pozY = -(mapSize / 2) + 0.5f + y;
				for (unsigned short x = 0; x < mapSize; ++x) {
					const GLfloat posX = -(mapSize / 2) + 0.5f + x;
					const CCell& cell = _Map.GetCell(x, y);
					if (cell.GetCellType() == CCell::CTReceiver)
						_Explosions.push_back(CExplosion(_Game, posX, -pozY));
				}
			}
		}

		//Render winner explosions
		glPushMatrix();
			const float scale = 10.0f / static_cast<float>(mapSize);
			glScalef(scale, scale, scale);
			for (list<CExplosion>::iterator itExpl = _Explosions.begin(); itExpl != _Explosions.end(); ++itExpl)
				itExpl->Render();
		glPopMatrix();

		redarawIsNeeded = true;
	}

	if (redarawIsNeeded)
		_Game.WinManager().PostRedisplay();
}


void CModePuzzle::OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn)
{
	if (!_Map.IsGameOver() && mouseX > -5.0f && mouseX < 5.0f && mouseY > -5.0f && mouseY < 5.0f) {		//point in game field
		bool redrawNeeded = false;

		const float scale = GetMapScaleFactor();
		const unsigned short x = static_cast<unsigned short>((mouseX + 5.0f) / scale);
		const unsigned short y = static_cast<const unsigned short>(_Map.GetMapSize()) - 1 - static_cast<unsigned short>((mouseY + 5.0f) / scale);

		CCell& cell = _Map.GetCell(x, y);
		if (cell.GetCellType() != CCell::CTFree) {
			switch (btn) {
				case MouseButton_Middle:
					cell.ReverseLock();
					redrawNeeded = true;
					break;
				case MouseButton_Left:
				case MouseButton_Right:
					cell.Rotate(btn != MouseButton_Left);
					_Map.DefineConnectStatus();
					redrawNeeded = true;
					break;
				default:
					break;
			}
		}

		if (redrawNeeded)
			_Game.WinManager().PostRedisplay();
	}
}


bool CModePuzzle::RenderPuzzle(const float transition)
{
	const unsigned short mapSize = static_cast<const unsigned short>(_Map.GetMapSize());

	//Set scale
	glPushMatrix();
	const float scale = GetMapScaleFactor();
	glScalef(scale, scale, scale);

	bool redarawIsNeeded = false;

	for (unsigned short y = 0; y < mapSize; ++y) {
		const float pozY = -(mapSize / 2) + 0.5f + y;
		for (unsigned short x = 0; x < mapSize; ++x) {
			const float posX = -(mapSize / 2) + 0.5f + x;
			CCell& cell = _Map.GetCell(x, y);
			const bool activeState = cell.IsActive();
			glPushMatrix();
				glTranslatef(posX, -pozY, 0.0f);

				//Draw tube
				if (cell.GetCellType() != CCell::CTFree) {
					for (char shadow = 1; shadow >= 0; --shadow) {
						glPushMatrix();
							if (!shadow)
								glColor4f(1.0f, 1.0, 1.0f, transition);
							else {
								glColor4f(0.0f, 0.0, 0.0f, transition / 2.0f);
								glTranslatef(0.05f, -0.05f, 0.0f);
							}
							if (cell.ProcessRotation()) {
								redarawIsNeeded = true;
								if (_UseSound && _Game.UserSettings().Sound)
									_Game.SoundBank().Play(CSoundBank::SndClatz);
							}
							glRotatef(cell.GetAngle(), 0.0f, 0.0f, 1.0f);

							CTextureBank::TextureType texType = CTextureBank::TexCounter;
							switch (cell.GetTubeType()) {
								case CCell::TTHalf:		texType = (activeState ? CTextureBank::TexTubeHalfActive : CTextureBank::TexTubeHalfPassive);	break;
								case CCell::TTStraight:	texType = (activeState ? CTextureBank::TexTubeStrActive : CTextureBank::TexTubeStrPassive);	break;
								case CCell::TTCurved:	texType = (activeState ? CTextureBank::TexTubeCrvActive : CTextureBank::TexTubeCrvPassive);	break;
								case CCell::TTJoiner:	texType = (activeState ? CTextureBank::TexTubeJnrActive : CTextureBank::TexTubeJnrPassive);	break;
								default:
									assert(false && "Unknown tube type");
									break;
							}
							RenderCell(texType);
							redarawIsNeeded |= cell.IsRotationInProgress();
						glPopMatrix();
					}
				}

				//Draw objects
				switch (cell.GetCellType()) {
					case CCell::CTFree:	//Free cell
					case CCell::CTTube:	//Will be drawn later
						break;
					case CCell::CTSender:
						RenderCell(CTextureBank::TexSender);
						break;
					case CCell::CTReceiver:
						RenderCell(activeState ? CTextureBank::TexReceiverActive : CTextureBank::TexReceiverPassive);
						break;
					default:
						assert(false && "Unknown object type");
						break;
				}

				//Draw lock
				if (cell.IsLocked()) {
					glDisable(GL_DEPTH_TEST);
						RenderCell(CTextureBank::TexLock);
					glEnable(GL_DEPTH_TEST);
				}
			glPopMatrix();
		}
	}

	glPopMatrix();

	//Restore after 'reset by rotate'
	if (!_UseSound && !redarawIsNeeded)
		_UseSound = true;

	if (redarawIsNeeded) {
		_Map.DefineConnectStatus();
		if (_Map.IsGameOver())
			_Game.SoundBank().Play(CSoundBank::SndComplete);
	}

	glColor4f(1.0f, 1.0, 1.0f, 1.0f);
	return redarawIsNeeded;
}


void CModePuzzle::RenderCell(const CTextureBank::TextureType type) const
{
	static const float vertices[] =			{ -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	static const short texture[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned short indices[] =	{ 0, 1, 2, 0, 2, 3 };

 	glBindTexture(GL_TEXTURE_2D, _Game.TextureBank().Get(type));
 	glVertexPointer(2, GL_FLOAT, 0, vertices);
 	glTexCoordPointer(2, GL_SHORT, 0, texture);
 	glDrawElements(GL_TRIANGLES, (sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_SHORT, indices);
}
