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
:	m_WinManager(NULL),
	m_ActiveMode(Puzzle),
	m_NextMode(Undefined),
	m_TrnPhase(FirstPhase),
	m_TrnStartTime(0),
	m_NextMapId(0),
	m_RenewMap(false),
	m_LoadMap(false),
	m_ModePuzzle(*this),
	m_ModeSettings(*this)
{
	m_Settings.Load();
}


void CGame::Initialize(CWinManager& winMgr)
{
	m_WinManager = &winMgr;

	//Configure buttons
	m_BtnPuzzle.reserve(4);
	m_BtnPuzzle.push_back(CButton( 2.0f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT,CTextureBank::TexButtonNext, PW_BUTTONID_NEXT));
	m_BtnPuzzle.push_back(CButton(-3.0f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonPrev, PW_BUTTONID_PREV));
	m_BtnPuzzle.push_back(CButton(PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonReset, PW_BUTTONID_RESET));
	m_BtnPuzzle.push_back(CButton(PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonSett, PW_BUTTONID_SETT));

	m_BtnSettings.reserve(2);
	m_BtnSettings.push_back(CButton(PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonOK, PW_BUTTONID_OK));
	m_BtnSettings.push_back(CButton(PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::TexButtonCancel, PW_BUTTONID_CANCEL));

	m_ModePuzzle.LoadMap();
	m_ModeSettings.Initialize();

	//First launch - make first transition (to make a new map)
	BeginTransition(Puzzle, SecondPhase);
}


void CGame::Finalize()
{
	m_ModePuzzle.SaveMap();
	m_Settings.Save();
}


void CGame::RenderScene(const float mouseX, const float mouseY)
{
	RenderEnvironment();

	//Transition calculate
	float trnPhase = 1.0f;
	if (TransitionInProgress()) {
		if (CSynchro::GetPhase(m_TrnStartTime, 500, trnPhase)) {
			if (m_TrnPhase == FirstPhase)
				trnPhase = 1.0f - trnPhase;	//Invert phase
		}
		else {
			//Transition phase complete
			if (m_TrnPhase == FirstPhase) {
				trnPhase = 0.0f;
				m_TrnPhase = SecondPhase;

				assert((!m_RenewMap && !m_LoadMap) || (m_RenewMap && !m_LoadMap) || (!m_RenewMap && m_LoadMap));
				if (m_RenewMap) {
					m_Settings.SetCurrentMapId(m_NextMapId);
					m_ModePuzzle.RenewMap();
					m_RenewMap = false;
				}
				else if (m_LoadMap) {
					m_ModePuzzle.LoadMap();
					m_LoadMap = false;
				}

				m_TrnStartTime = CSynchro::GetTick();	//second phase begin
				m_ActiveMode = m_NextMode;
				m_NextMode = Undefined;
			}
			else {
				//Second phase finished
				m_TrnStartTime = 0;
			}

		}
		m_WinManager->PostRedisplay();
	}

	const vector<CButton>* activeBtns = NULL;

	if (m_ActiveMode == Puzzle) {
		m_ModePuzzle.Render(trnPhase);
		activeBtns = &m_BtnPuzzle;
	}
 	else if (m_ActiveMode == Options) {
		m_ModeSettings.Render(mouseX, mouseY, trnPhase);
		activeBtns = &m_BtnSettings;
	}

	//Buttons
	assert(activeBtns != NULL);
	glColor4f(1.0f, 1.0, 1.0f, trnPhase);
	for (vector<CButton>::const_iterator itBtn = activeBtns->begin(); itBtn != activeBtns->end(); ++itBtn)
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
 			glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvBkgr));
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
	glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvTitle));
	glVertexPointer(2, GL_FLOAT, 0, vertTitle);
	glTexCoordPointer(2, GL_SHORT, 0, texTitle);
	glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_SHORT, plainInd);

	//Draw cell's background
	const unsigned short mapSize = static_cast<const unsigned short>(m_ModePuzzle.GetMapSize());
	static const float cellVertex[] = { -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
	glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexCellBackground));
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
		const vector<CButton>& btns = (m_ActiveMode == Puzzle ? m_BtnPuzzle : m_BtnSettings);
		for (vector<CButton>::const_iterator itBtn = btns.begin(); itBtn != btns.end(); ++itBtn) {
			if (itBtn->IsMouseOver(mouseX, mouseY)) {
				switch (itBtn->GetId()) {
					case PW_BUTTONID_NEXT:
					case PW_BUTTONID_PREV:
						RenewMap(m_Settings.GetCurrentMapId() + (itBtn->GetId() == PW_BUTTONID_NEXT ? 1 : -1));
						break;
					case PW_BUTTONID_RESET:
						m_ModePuzzle.ResetByRotate();
						m_WinManager->PostRedisplay();
						break;
					case PW_BUTTONID_SETT:
						m_ModeSettings.Reset();
						BeginTransition(Options);
						break;
					case PW_BUTTONID_OK:
						m_ModePuzzle.SaveMap();	//Save current map
						
						if (m_Settings.Size != m_ModeSettings.GetMapSize()) {
							m_Settings.Size = m_ModeSettings.GetMapSize();
							m_LoadMap = true;
							BeginTransition(Puzzle);
						}
						else if (m_Settings.Wrapping != m_ModeSettings.GetWrapMode()) {
							m_Settings.Wrapping = m_ModeSettings.GetWrapMode();
							RenewMap(m_Settings.GetCurrentMapId());
						}
						else
							BeginTransition(Puzzle);
						m_Settings.Sound = m_ModeSettings.GetSoundMode();
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

	if (m_ActiveMode == Puzzle)
		m_ModePuzzle.OnMouseButtonDown(mouseX, mouseY, btn);
	else if (m_ActiveMode == Options)
		m_ModeSettings.OnMouseButtonDown(mouseX, mouseY, btn);
}


void CGame::BeginTransition(const GameMode nextMode, const TransitionPhase startPhase /*= FirstPhase*/)
{
	m_NextMode = nextMode;
	m_TrnPhase = startPhase;
	m_TrnStartTime = CSynchro::GetTick();
	m_WinManager->PostRedisplay();
}


void CGame::OnKeyboardKeyDown(const char key)
{
	if (!TransitionInProgress() && key == 27) { //Esc
		if (m_ActiveMode == Puzzle)
			m_WinManager->PostExit();
		else if (m_ActiveMode == Options)
			BeginTransition(Puzzle);
	}
}


void CGame::OnMouseMove(const float /*mouseX*/, const float /*mouseY*/)
{
	m_WinManager->PostRedisplay();
}


void CGame::RenewMap(const unsigned long nextMapNum)
{
	m_NextMapId = (nextMapNum == 0 || nextMapNum > 99999999) ? 1 : nextMapNum;
	m_RenewMap = true;
	BeginTransition(Puzzle);
}
