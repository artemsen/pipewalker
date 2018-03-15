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
#define PW_BUTTON_TOP		-5.2f
#define PW_BUTTON_WIDTH		 1.0f
#define PW_BUTTON_HEIGHT	 1.0f
#define PW_BUTTON_LEFT		-5.0f
#define PW_BUTTON_RIGHT		 4.0f


CGame::CGame()
: m_Mode(Puzzle), m_TrnPhase(FirstPhase), m_TrnStartTime(0), m_NextMapId(1), m_ModePuzzle(&m_Map)
{
}


void CGame::DoMainLoop()
{
	//Configure buttons
	m_BtnPuzzle.reserve(4);
	m_BtnPuzzle.push_back(CButton( 2.0f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonNext), PW_BUTTONID_NEXT));
	m_BtnPuzzle.push_back(CButton(-3.0f, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonPrev), PW_BUTTONID_PREV));
	m_BtnPuzzle.push_back(CButton(PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonReset), PW_BUTTONID_RESET));
	m_BtnPuzzle.push_back(CButton(PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonSett), PW_BUTTONID_SETT));

	m_BtnSettings.reserve(2);
	m_BtnSettings.push_back(CButton(PW_BUTTON_RIGHT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonOK), PW_BUTTONID_OK));
	m_BtnSettings.push_back(CButton(PW_BUTTON_LEFT, PW_BUTTON_TOP, PW_BUTTON_WIDTH, PW_BUTTON_HEIGHT, CTextureBank::Get(CTextureBank::TexButtonCancel), PW_BUTTONID_CANCEL));

	//Initialize game map
	if (CSettings::Settings.Id && !CSettings::Settings.State.empty())
		m_Map.LoadMap(CSettings::Settings.Size, CSettings::Settings.Id, CSettings::Settings.State.c_str());
	else
		m_Map.New(CSettings::Settings.Size, 1);
	m_NextMapId = m_Map.GetMapID();

	m_ModeSettings.Initialize();

	//First launch - make first transition (to make a new map)
	BeginTransition(Puzzle);
	m_TrnPhase = SecondPhase;

	bool exitProgram = false;

	while (!exitProgram) {
		SDL_Event event;
		if (SDL_WaitEvent(&event) == 0)
			throw CException("SDL_WaitEvent failed: ", SDL_GetError());
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				OnMouseClick(event.button.button);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					exitProgram = true;
				break;
			case SDL_MOUSEMOTION:
			case SDL_VIDEOEXPOSE:
				RenderScene();
				break;
			case SDL_QUIT:
				exitProgram = true;
				break;
			default:
				break;
		}
	}

	//Save map state to settings
	CSettings::Settings.Id = m_Map.GetMapID();
	CSettings::Settings.Size = static_cast<MapSize>(m_Map.GetMapSize());
	CSettings::Settings.State = m_Map.SaveMap();
}


bool CGame::GetMousePosition(float& mouseX, float& mouseY)
{
	//Get current mouse pointer position
	int mousePosX, mousePosY;
	SDL_GetMouseState(&mousePosX, &mousePosY);

	GLint glViewport[4];
	glGetIntegerv(GL_VIEWPORT, glViewport);

	if (mouseX < 0 || mouseX > glViewport[2] || mouseY < 0 || mouseY > glViewport[3])
		return false;

	GLdouble glProjection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, glProjection);

	GLdouble glModelView[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, glModelView);

	GLfloat depth;
	glReadPixels(mousePosX, glViewport[3] - mousePosY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	GLdouble worldX, worldY, worldZ;
	gluUnProject(static_cast<GLdouble>(mousePosX), static_cast<GLdouble>(glViewport[3] - mousePosY), depth, glModelView, glProjection, glViewport, &worldX, &worldY,& worldZ);

	mouseX = static_cast<float>(worldX);
	mouseY = static_cast<float>(worldY);
	return true;
}


void CGame::PostRedrawEvent()
{
	const int exposeEventNum = SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_VIDEOEXPOSEMASK);
	if (exposeEventNum <= 0) {
		static int lastRedraw = CSynchro::GetTick();
		if (CSynchro::GetTick() - lastRedraw < 10)
			SDL_Delay(10);
		lastRedraw = CSynchro::GetTick();

		SDL_Event event;
		event.type = SDL_VIDEOEXPOSE;
		SDL_PushEvent(&event);
	}
}


void CGame::RenderScene()
{
	bool redarawIsNeeded = false;

	float mouseX = 0.0f, mouseY = 0.0f;
	GetMousePosition(mouseX, mouseY);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	RenderEnvironment();

	//Transition calculate
	float trnPhase = 1.0f;
 	if (m_Mode != Puzzle && m_Mode != Settings) {
 		if (!CSynchro::GetPhase(m_TrnStartTime, 500, trnPhase)) {
 			//Transition phase complete
			if (m_TrnPhase == FirstPhase) {
				trnPhase = 0.0f;
				m_TrnPhase = SecondPhase;

				if (m_Mode == Puzzle2Puzzle || CSettings::Settings.Size != m_Map.GetMapSize())
					m_Map.New(CSettings::Settings.Size, m_NextMapId);

				m_TrnStartTime = CSynchro::GetTick();	//second phase begin
			}
			else {
				if (m_Mode == Puzzle2Settings)
					m_Mode = Settings;
				else
					m_Mode = Puzzle;
			}
 		}
		else if (m_TrnPhase == FirstPhase)
			trnPhase = 1.0f - trnPhase;	//Invert phase

		redarawIsNeeded = true;
 	}
	const vector<CButton>* activeBtns = NULL;

	if (m_Mode == Puzzle || m_Mode == Puzzle2Puzzle ||
		(m_TrnPhase == FirstPhase && m_Mode == Puzzle2Settings) ||
		(m_TrnPhase == SecondPhase && m_Mode == Settings2Puzzle)) {
			redarawIsNeeded |= m_ModePuzzle.Render(trnPhase);
			activeBtns = &m_BtnPuzzle;
	}
 	else if (m_Mode == Settings ||
		(m_TrnPhase == FirstPhase && m_Mode == Settings2Puzzle) ||
		(m_TrnPhase == SecondPhase && m_Mode == Puzzle2Settings)) {
			redarawIsNeeded |= m_ModeSettings.Render(mouseX, mouseY, trnPhase);
			activeBtns = &m_BtnSettings;
	}

	//Buttons
	assert(activeBtns != NULL);
	glColor4f(1.0f, 1.0, 1.0f, trnPhase);
	for (vector<CButton>::const_iterator itBtn = activeBtns->begin(); itBtn != activeBtns->end(); ++itBtn)
		itBtn->Render(mouseX, mouseY);
	glColor4f(1.0f, 1.0, 1.0f, 1.0f);


	SDL_GL_SwapBuffers();

	if (redarawIsNeeded)
		PostRedrawEvent();
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
	static const short texBkgr[] =	{ 0, 4, 0, 0, 3, 0, 3, 4 };
	static const unsigned int plainInd[] =	{ 0, 1, 2, 0, 2, 3 };

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
			glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_INT, plainInd);

			const float vertTitle[] =		{ 10.0f, wndHeight - 5.0f, 10.0f, wndHeight - 55.0f, wndWidth - 10.0f, wndHeight - 55.0f, wndWidth - 10.0f, wndHeight - 5.0f };
			static const short texTitle[] =	{ 0, 1, 0, 0, 1, 0, 1, 1 };
			glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvTitle));
			glVertexPointer(2, GL_FLOAT, 0, vertTitle);
			glTexCoordPointer(2, GL_SHORT, 0, texTitle);
			glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_INT, plainInd);

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	//Draw cell's background
	const unsigned short mapSize = static_cast<const unsigned short>(m_Map.GetMapSize());
	static const float cellVertex[] = { -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
	glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexCellBackground));
	glVertexPointer(2, GL_FLOAT, 0, cellVertex);
	const short cellTexture[] =	{ 0, mapSize, 0, 0, mapSize, 0, mapSize, mapSize };
	glTexCoordPointer(2, GL_SHORT, 0, cellTexture);
	glDrawElements(GL_TRIANGLES, (sizeof(plainInd) / sizeof(plainInd[0])), GL_UNSIGNED_INT, plainInd);
}


void CGame::OnMouseClick(const Uint8 button)
{
	float mouseX = 0.0f, mouseY = 0.0f;
	if (!GetMousePosition(mouseX, mouseY))
		return;

	if (button == SDL_BUTTON_LEFT && (m_Mode == Puzzle || m_Mode == Settings)) {
		const vector<CButton>& btns = (m_Mode == Puzzle ? m_BtnPuzzle : m_BtnSettings);
		for (vector<CButton>::const_iterator itBtn = btns.begin(); itBtn != btns.end(); ++itBtn) {
			if (itBtn->IsMouseOver(mouseX, mouseY)) {
				switch (itBtn->GetId()) {
					case PW_BUTTONID_NEXT:
					case PW_BUTTONID_PREV:
						m_NextMapId += (itBtn->GetId() == PW_BUTTONID_NEXT ? 1 : -1);
						if (m_NextMapId == 0 || m_NextMapId > 99999999)
							m_NextMapId = 1;
						m_ModePuzzle.ResetExplosions();
						BeginTransition(Puzzle);
						break;
					case PW_BUTTONID_RESET:
						m_ModePuzzle.ResetExplosions();
						m_Map.ResetByRotate();
						break;
					case PW_BUTTONID_SETT:
						m_ModeSettings.SetMapSize(CSettings::Settings.Size);
						BeginTransition(Settings);
						break;
					case PW_BUTTONID_OK:
						CSettings::Settings.Size = m_ModeSettings.GetMapSize();
						if (CSettings::Settings.Size != m_Map.GetMapSize())
							m_ModePuzzle.ResetExplosions();
						BeginTransition(Puzzle);
						break;
					case PW_BUTTONID_CANCEL:
						BeginTransition(Puzzle);
						break;
					default:
						assert(false && "Unhandled button id");
				}
			}
		}
	}

	if (m_Mode == Puzzle)
		m_ModePuzzle.OnMouseClick(button, mouseX, mouseY);
	else if (m_Mode == Settings)
		m_ModeSettings.OnMouseClick(button, mouseX, mouseY);

	PostRedrawEvent();
}


void CGame::BeginTransition(const Mode newMode)
{
	if (newMode == Puzzle)
		m_Mode = (m_Mode == Puzzle ? Puzzle2Puzzle : Settings2Puzzle);
	else if (newMode == Settings)
		m_Mode = Puzzle2Settings;
	m_TrnPhase = FirstPhase;
	m_TrnStartTime = CSynchro::GetTick();
}
