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

#include "game.h"
#include "texture.h"
#include "synchro.h"
#include "settings.h"

//Button identifiers
#define PW_BUTTONID_NEXT		1
#define PW_BUTTONID_PREV		2
#define PW_BUTTONID_RESET		3
#define PW_BUTTONID_SETT		4
#define PW_BUTTONID_OK			5
#define PW_BUTTONID_CANCEL		6

//Common button properties
#define PW_BUTTON_TOP		-5.1f
#define PW_BUTTON_WIDTH		 1.0f
#define PW_BUTTON_HEIGHT	 1.0f
#define PW_BUTTON_LEFT		-5.0f
#define PW_BUTTON_RIGHT		 4.0f

#ifdef _MSC_VER
#pragma warning(disable: 4355)	//'this' : used in base member initializer list
#endif // WIN32


CGame::CGame()
:	_WinManager(NULL),
	_ActiveMode(Puzzle),
	_NextMode(Undefined),
	_TrnPhase(FirstPhase),
	_TrnStartTime(0),
	_NextMapId(0),
	_RenewMap(false),
	_LoadMap(false),
	_ModePuzzle(*this),
	_ModeSettings(*this)
{
}


void CGame::Initialize(CWinManager& winMgr)
{
	_WinManager = &winMgr;

	//Load settings
	_UserSettings.Load();
	_GameSettings.Load();

	//Initialize texture/sound banks
	ReloadTextures();
	_SoundBank.Load();

	//Configure buttons
	_BtnPuzzle.push_back(CButton(*this, 2.2f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT,CTextureBank::TexButtonNext, PW_BUTTONID_NEXT));
	_BtnPuzzle.push_back(CButton(*this, -3.2f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonPrev, PW_BUTTONID_PREV));
	_BtnPuzzle.push_back(CButton(*this, PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonReset, PW_BUTTONID_RESET));
	_BtnPuzzle.push_back(CButton(*this, PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonSett, PW_BUTTONID_SETT));

	_BtnSettings.push_back(CButton(*this, PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonOK, PW_BUTTONID_OK));
	_BtnSettings.push_back(CButton(*this, PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonCancel, PW_BUTTONID_CANCEL));

	_ModePuzzle.LoadMap();
	_ModeSettings.Initialize();

	//First launch - make first transition (to make a new map)
	BeginTransition(Puzzle, SecondPhase);
}


void CGame::Finalize()
{
	_ModePuzzle.SaveMap();
	_UserSettings.Save();
}


void CGame::ReloadTextures()
{
	if (_UserSettings.ThemeId >= _GameSettings.Themes.size())
		_UserSettings.ThemeId = 0;

	string textFileName = DIR_GAMEDATA;
	textFileName += _GameSettings.Themes[_UserSettings.ThemeId].TextureFile;
	_TextureBank.Load(textFileName.c_str());
	_RenderText.Load();
}


void CGame::RenderScene(const float mouseX, const float mouseY)
{
	RenderEnvironment();

	//Transition calculate
	float trnPhase = 1.0f;
	if (TransitionInProgress()) {
		if (CSynchro::GetPhase(_TrnStartTime, 500, trnPhase)) {
			if (_TrnPhase == FirstPhase)
				trnPhase = 1.0f - trnPhase;	//Invert phase
		}
		else {
			//Transition phase complete
			if (_TrnPhase == FirstPhase) {
				trnPhase = 0.0f;
				_TrnPhase = SecondPhase;

				assert((!_RenewMap && !_LoadMap) || (_RenewMap && !_LoadMap) || (!_RenewMap && _LoadMap));
				if (_RenewMap) {
					_UserSettings.SetCurrentMapId(_NextMapId);
					_ModePuzzle.RenewMap();
					_RenewMap = false;
				}
				else if (_LoadMap) {
					_ModePuzzle.LoadMap();
					_LoadMap = false;
				}

				_TrnStartTime = CSynchro::GetTick();	//second phase begin
				_ActiveMode = _NextMode;
				_NextMode = Undefined;
			}
			else {
				//Second phase finished
				_TrnStartTime = 0;
			}

		}
		_WinManager->PostRedisplay();
	}

	const list<CButton>* activeBtns = NULL;

	if (_ActiveMode == Puzzle) {
		_ModePuzzle.Render(trnPhase);
		activeBtns = &_BtnPuzzle;
	}
 	else if (_ActiveMode == Options) {
		_ModeSettings.Render(mouseX, mouseY, trnPhase);
		activeBtns = &_BtnSettings;
	}

	//Buttons
	assert(activeBtns != NULL);
	glColor4f(1.0f, 1.0, 1.0f, trnPhase);
	for (list<CButton>::const_iterator itBtn = activeBtns->begin(); itBtn != activeBtns->end(); ++itBtn)
		itBtn->Render(mouseX, mouseY);
	glColor4f(1.0f, 1.0, 1.0f, 1.0f);
}


void CGame::RenderEnvironment()
{
	//Render environment background
	glDisable(GL_DEPTH_TEST);
 	GLint glViewport[4];
 	glGetIntegerv(GL_VIEWPORT, glViewport);
 	const float wndWidth = static_cast<float>(glViewport[2]);
 	const float wndHeight = static_cast<float>(glViewport[3]);
 	const float vertBkgr[] = { 0.0f, wndHeight, 0.0f, 0.0f, wndWidth, 0.0f, wndWidth, wndHeight };
 	static const short texBkgr[] = { 0, 8, 0, 0, 6, 0, 6, 8 };
 	static const unsigned short plainInd[] = { 0, 1, 2, 0, 2, 3 };

 	glMatrixMode(GL_PROJECTION);
 	glPushMatrix();
 		glLoadIdentity();
 		glOrtho(0, glViewport[2], 0, glViewport[3], -1, 1);
 		glMatrixMode(GL_MODELVIEW);
 		glPushMatrix();
 			glLoadIdentity();
 			glBindTexture(GL_TEXTURE_2D, _TextureBank.Get(CTextureBank::TexEnvBkgr));
 			glVertexPointer(2, GL_FLOAT, 0, vertBkgr);
 			glTexCoordPointer(2, GL_SHORT, 0, texBkgr);
 			glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_SHORT, plainInd);
			glMatrixMode(GL_PROJECTION);
 		glPopMatrix();
 		glMatrixMode(GL_MODELVIEW);
 	glPopMatrix();
 	glEnable(GL_DEPTH_TEST);

	//Draw title
	static const float vertTitle[] = { -5.0f, 6.1f, -5.0f, 5.1f, 5.0f, 5.1f, 5.0f, 6.1f };
	static const short texTitle[] =	 { 0, 1, 0, 0, 1, 0, 1, 1 };
	glBindTexture(GL_TEXTURE_2D, _TextureBank.Get(CTextureBank::TexEnvTitle));
	glVertexPointer(2, GL_FLOAT, 0, vertTitle);
	glTexCoordPointer(2, GL_SHORT, 0, texTitle);
	glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_SHORT, plainInd);

	//Draw cell's background
	const unsigned short mapSize = static_cast<const unsigned short>(_ModePuzzle.GetMapSize());
	static const float cellVertex[] = { -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
	glBindTexture(GL_TEXTURE_2D, _TextureBank.Get(CTextureBank::TexCellBackground));
	glVertexPointer(2, GL_FLOAT, 0, cellVertex);
	const short cellTexture[] =	{ 0, mapSize, 0, 0, mapSize, 0, mapSize, mapSize };
	glTexCoordPointer(2, GL_SHORT, 0, cellTexture);
	glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_SHORT, plainInd);
}


void CGame::OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn)
{
	if (TransitionInProgress())
		return;

	if (btn == MouseButton_Left) {
		const list<CButton>& btns = (_ActiveMode == Puzzle ? _BtnPuzzle : _BtnSettings);
		for (list<CButton>::const_iterator itBtn = btns.begin(); itBtn != btns.end(); ++itBtn) {
			if (itBtn->IsMouseOver(mouseX, mouseY)) {
				switch (itBtn->GetId()) {
					case PW_BUTTONID_NEXT:
					case PW_BUTTONID_PREV:
						RenewMap(_UserSettings.GetCurrentMapId() + (itBtn->GetId() == PW_BUTTONID_NEXT ? 1 : -1));
						break;
					case PW_BUTTONID_RESET:
						_ModePuzzle.ResetByRotate();
						_WinManager->PostRedisplay();
						break;
					case PW_BUTTONID_SETT:
						_ModeSettings.Reset();
						BeginTransition(Options);
						break;
					case PW_BUTTONID_OK:
						_ModePuzzle.SaveMap();	//Save current map
						
						if (_UserSettings.Size != _ModeSettings.GetMapSize()) {
							_UserSettings.Size = _ModeSettings.GetMapSize();
							_LoadMap = true;
							BeginTransition(Puzzle);
						}
						else if (_UserSettings.Wrapping != _ModeSettings.GetWrapMode()) {
							_UserSettings.Wrapping = _ModeSettings.GetWrapMode();
							RenewMap(_UserSettings.GetCurrentMapId());
						}
						else
							BeginTransition(Puzzle);
						_UserSettings.Sound = _ModeSettings.GetSoundMode();
						break;
					case PW_BUTTONID_CANCEL:
						BeginTransition(Puzzle);
						break;
					default:
						assert(false && "Unregistered button id");
				}
			}
		}
	}

	if (_ActiveMode == Puzzle)
		_ModePuzzle.OnMouseButtonDown(mouseX, mouseY, btn);
	else if (_ActiveMode == Options)
		_ModeSettings.OnMouseButtonDown(mouseX, mouseY, btn);
}


void CGame::BeginTransition(const GameMode nextMode, const TransitionPhase startPhase /*= FirstPhase*/)
{
	_NextMode = nextMode;
	_TrnPhase = startPhase;
	_TrnStartTime = CSynchro::GetTick();
	_WinManager->PostRedisplay();
}


void CGame::OnKeyboardKeyDown(const char key)
{
	if (!TransitionInProgress() && key == 27) { //Esc
		if (_ActiveMode == Puzzle)
			_WinManager->PostExit();
		else if (_ActiveMode == Options)
			BeginTransition(Puzzle);
	}
}


void CGame::OnMouseMove(const float /*mouseX*/, const float /*mouseY*/)
{
	_WinManager->PostRedisplay();
}


void CGame::RenewMap(const unsigned long nextMapNum)
{
	_NextMapId = (nextMapNum == 0 || nextMapNum > 99999999) ? 1 : nextMapNum;
	_RenewMap = true;
	BeginTransition(Puzzle);
}
