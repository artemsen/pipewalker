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
#include "sound.h"
#include "synchro.h"
#include "serializer.h"

//Button identifiers
#define PW_BTN_NEXT		1
#define PW_BTN_PREV		2
#define PW_BTN_RESET	3
#define PW_BTN_INFO		4
#define PW_BTN_OK		5

//! Plain texture coordinates
static const short PlainTex[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };
//! Plain indices
static const unsigned int PlainInd[] =	{ 0, 1, 2, 0, 2, 3 };


CGame::CGame()
: m_NextMapId(1), m_Mode(InfoToPlay), m_ModeStartTime(0)
{
}


void CGame::StartGame()
{
	CGame game;

	const float btnY = -5.2f;
	const float btnW = 1.0f;
	const float btnH = 1.0f;
	game.m_BtnNext.Create( 2.0f, btnY, btnW, btnH, CTextureBank::Get(CTextureBank::TexButtonNext), PW_BTN_NEXT);
	game.m_BtnPrev.Create(-3.0f, btnY, btnW, btnH, CTextureBank::Get(CTextureBank::TexButtonPrev), PW_BTN_PREV);
	game.m_BtnReset.Create(-5.0f, btnY, btnW, btnH, CTextureBank::Get(CTextureBank::TexButtonReset), PW_BTN_RESET);
	game.m_BtnInfo.Create(4.0f, btnY, btnW, btnH, CTextureBank::Get(CTextureBank::TexButtonInfo), PW_BTN_INFO);
	game.m_BtnOk.Create(4.0f, btnY, btnW, btnH, CTextureBank::Get(CTextureBank::TexButtonOK), PW_BTN_OK);
	game.DoMainLoop();
}


void CGame::DoMainLoop()
{
	//Load last saved map
	string mapDescr;
	if (CSerializer::Load(m_NextMapId, mapDescr))
		m_Map.LoadMap(m_NextMapId, mapDescr.c_str());
	else
		m_Map.New(m_NextMapId);

	//First launch - make first transition (to make a new map)
	m_ModeStartTime = CSynchro::GetTick();

	bool exitProgram = false;

	while (!exitProgram) {
		SDL_Event event;
		if (SDL_WaitEvent(&event) == 0)
			throw string("SDL_WaitEvent failed: ") + SDL_GetError();
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				OnMouseClick(event.button.button);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					exitProgram = true;
				else if (event.key.keysym.sym == SDLK_n) {
					if (m_NextMapId < 99999999) {
						++m_NextMapId;
						m_Explosions.clear();
						m_Mode = PlayToPlay;
						m_ModeStartTime = CSynchro::GetTick();
					}
					PostRedrawEvent();
				}
				else if (event.key.keysym.sym == SDLK_r) {
					m_Map.ResetByRotate();
					m_Explosions.clear();
					PostRedrawEvent();
				}
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

	//Save state
	CSerializer::Save(m_Map.GetMapID(), m_Map.SaveMap().c_str());
}


bool CGame::GetMousePosition(float& posX, float& posY)
{
	//Get current mouse pointer position
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	GLint glViewport[4];
	glGetIntegerv(GL_VIEWPORT, glViewport);

	if (mouseX < 0 || mouseX > glViewport[2] || mouseY < 0 || mouseY > glViewport[3])
		return false;

	GLdouble glProjection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, glProjection);

	GLdouble glModelView[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, glModelView);

	GLfloat depth;
	glReadPixels(mouseX, glViewport[3] - mouseY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	GLdouble worldX, worldY, worldZ;
	gluUnProject(static_cast<GLdouble>(mouseX), static_cast<GLdouble>(glViewport[3] - mouseY), depth, glModelView, glProjection, glViewport, &worldX, &worldY,& worldZ);

	posX = static_cast<float>(worldX);
	posY = static_cast<float>(worldY);
	return true;
}


void CGame::PostRedrawEvent()
{
	SDL_Event event;
	event.type = SDL_VIDEOEXPOSE;
	SDL_PushEvent(&event);
}


void CGame::RenderScene()
{
	bool redarawIsNeeded = false;

	float mouseX = 0.0f, mouseY = 0.0f;
	GetMousePosition(mouseX, mouseY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	redarawIsNeeded |= RenderEnvironment(mouseX, mouseY);

	//Transition calculate
	float modeChanging = 1.0f;
	if (m_Mode != Info && m_Mode != Play) {
		if (!CSynchro::GetPhase(m_ModeStartTime, 1000, modeChanging)) {
			//Transition complete
			if (m_Mode == InfoToPlay || m_Mode == PlayToPlay)
				m_Mode = Play;
			else if (m_Mode == PlayToInfo)
				m_Mode = Info;
			redarawIsNeeded = true;
		}
	}

	if (m_Mode != Info /*any except info*/) {
		float scale = 1.0f;
		float rotate = 0.0f;
		if (m_Mode == InfoToPlay) {
			scale += 20.0f * (1.0f - modeChanging);
			rotate = (1.0f - modeChanging) * -360.0f;
		}
		else if (m_Mode == PlayToInfo) {
			scale += 20.0f * modeChanging;
			rotate = modeChanging * 360.0f;
		}
		else if (m_Mode == PlayToPlay) {
			if (modeChanging < 0.5f) {
				scale += 20.0f * modeChanging;
				rotate = modeChanging * 360.0f;
			}
			else {
				if (m_NextMapId != m_Map.GetMapID())
					m_Map.New(m_NextMapId);
				scale += 20.0f * (1.0f - modeChanging);
				rotate = (1.0f - modeChanging) * -360.0f;
			}
		}

		glPushMatrix();
			if (m_Mode == InfoToPlay || m_Mode == PlayToInfo || m_Mode == PlayToPlay) {
				glScalef(scale, scale, 1.0f);
				glRotatef(rotate, 0.0f, 0.0f, 1.0f);
			}
			redarawIsNeeded |= RenderPuzzle();
		glPopMatrix();

		if (m_Map.IsGameOver()) {
			if (m_Explosions.empty()) {
				//Create explosions
				const unsigned short mapSize = m_Map.GetMapSize();
				for (unsigned short y = 0; y < mapSize; y++) {
					const GLfloat pozY = -(mapSize / 2) + 0.5f + y;
					for (unsigned short x = 0; x < mapSize; x++) {
						const GLfloat posX = -(mapSize / 2) + 0.5f + x;
						const CCell& cell = m_Map.GetCell(x, y);
						if (cell.GetCellType() == CCell::CTReceiver)
							m_Explosions.push_back(CExplosion(posX, -pozY));
					}
				}
			}

			//Render winner explosions
			for (list<CExplosion>::iterator itExpl = m_Explosions.begin(); itExpl != m_Explosions.end(); ++itExpl)
				itExpl->Render();

			redarawIsNeeded = true;
		}
	}

	if (m_Mode == InfoToPlay || m_Mode == PlayToInfo || m_Mode == Info) {
		float scale = 1.0f;
		float rotate = 0.0f;
		if (m_Mode == InfoToPlay) {
			scale += 20.0f * modeChanging;
			rotate = modeChanging * -360.0f;
		}
		else if (m_Mode == PlayToInfo) {
			scale += 20.0f * (1.0f - modeChanging);
			rotate = (1.0f - modeChanging) * -360.0f;
		}

		//Draw info pane
		static const float CellVertex[] = { -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
		glPushMatrix();
			if (m_Mode == InfoToPlay || m_Mode == PlayToInfo) {
				glScalef(scale, scale, 1.0f);
				glRotatef(rotate, 0.0f, 0.0f, 1.0f);
			}
			glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvInfo));
			glVertexPointer(2, GL_FLOAT, 0, CellVertex);
			glTexCoordPointer(2, GL_SHORT, 0, PlainTex);
			glDrawElements(GL_TRIANGLES, (sizeof(PlainInd) / sizeof(PlainInd[0])), GL_UNSIGNED_INT, PlainInd);
		glPopMatrix();

	}

	SDL_GL_SwapBuffers();

	redarawIsNeeded |= (m_Mode != Info && m_Mode != Play);

	if (redarawIsNeeded)
		PostRedrawEvent();
}


bool CGame::RenderEnvironment(const float mouseX, const float mouseY)
{
	//Render environment background
	glDisable(GL_DEPTH_TEST);
	GLint glViewport[4];
	glGetIntegerv(GL_VIEWPORT, glViewport);
	const float wndWidth = static_cast<float>(glViewport[2]);
	const float wndHeight = static_cast<float>(glViewport[3]);
	const float vertBkgr[] = { 0.0f, wndHeight, 0.0f, 0.0f, wndWidth, 0.0f, wndWidth, wndHeight };
	static const short texBkgr[] =	{ 0, 5, 0, 0, 3, 0, 3, 5 };
	
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
			glDrawElements(GL_TRIANGLES, (sizeof(PlainInd) / sizeof(PlainInd[0])), GL_UNSIGNED_INT, PlainInd);

			const float vertTitle[] = { 10.0f, wndHeight - 5.0f, 10.0f, wndHeight - 50.0f, wndWidth - 10.0f, wndHeight - 50.0f, wndWidth - 10.0f, wndHeight - 5.0f };
			glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexEnvTitle));
			glVertexPointer(2, GL_FLOAT, 0, vertTitle);
			glTexCoordPointer(2, GL_SHORT, 0, PlainTex);
			glDrawElements(GL_TRIANGLES, (sizeof(PlainInd) / sizeof(PlainInd[0])), GL_UNSIGNED_INT, PlainInd);

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	//Render map ID
	if (m_Mode == Play || m_Mode == PlayToPlay) {
		static const float MapIdVertex[] = { -0.2f, 0.4f, -0.2f, -0.4f, 0.2f, -0.4f, 0.2f, 0.4f };
		glPushMatrix();
			glTranslatef(-2.25f, -5.7f, 0.0f);
			char szMapId[64];
			sprintf(szMapId, "%i", m_Map.GetMapID());
			string mapIdAsString(szMapId);
			while (mapIdAsString.length() < 8)
				mapIdAsString = '0' + mapIdAsString;
			for (size_t i = 0; i < 8; ++i) {
				glTranslatef(0.5f, 0.0f, 0.0f);
				glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(static_cast<CTextureBank::TextureType>(CTextureBank::TexNum0 + (mapIdAsString[i] - '0'))));
				glVertexPointer(2, GL_FLOAT, 0, MapIdVertex);
				glTexCoordPointer(2, GL_SHORT, 0, PlainTex);
				glDrawElements(GL_TRIANGLES, (sizeof(PlainInd) / sizeof(PlainInd[0])), GL_UNSIGNED_INT, PlainInd);
			}
		glPopMatrix();
	}

	//Buttons
	if (m_Mode == Play) {
		m_BtnNext.Render(mouseX, mouseY);
		m_BtnPrev.Render(mouseX, mouseY);
		m_BtnReset.Render(mouseX, mouseY);
		m_BtnInfo.Render(mouseX, mouseY);
	}
	else if (m_Mode == Info) {
		m_BtnOk.Render(mouseX, mouseY);
	}

	//Draw cell's background
	const unsigned short mapSize = m_Map.GetMapSize();
	//Set scale
	glPushMatrix();
	float scale = 10.0f / static_cast<float>(mapSize);
	glScalef(scale, scale, scale);
	for (unsigned short y = 0; y < mapSize; ++y) {
		const float pozY = -(mapSize / 2) + 0.5f + y;
		for (unsigned short x = 0; x < mapSize; ++x) {
			const GLfloat posX = -(mapSize / 2) + 0.5f + x;
			glPushMatrix();
				glTranslatef(posX, -pozY, 0.0f);
				RenderCell(CTextureBank::TexCellBackground);
			glPopMatrix();
		}
	}
	glPopMatrix();

	return false;
}


bool CGame::RenderPuzzle()
{
	const unsigned short mapSize = m_Map.GetMapSize();

	//Set scale
	glPushMatrix();
	float scale = 10.0f / static_cast<float>(mapSize);
	glScalef(scale, scale, scale);

	bool redarawIsNeeded = false;

	for (unsigned short y = 0; y < mapSize; ++y) {
		const float pozY = -(mapSize / 2) + 0.5f + y;
		for (unsigned short x = 0; x < mapSize; ++x) {
			const float posX = -(mapSize / 2) + 0.5f + x;
			CCell& cell = m_Map.GetCell(x, y);
			const bool activeState = cell.IsActive();
			glPushMatrix();
				glTranslatef(posX, -pozY, 0.0f);

				//Draw tube
				if (cell.GetCellType() != CCell::CTFree) {
					for (char shadow = 1; shadow >= 0; --shadow) {
						glPushMatrix();
							if (!shadow)
								glColor4f(1.0f, 1.0, 1.0f, 1.0f);
							else {
								glColor4f(0.0f, 0.0, 0.0f, 0.6f);
								glTranslatef(0.05f, -0.05f, 0.0f);
							}
							//redarawIsNeeded |= cell.ProcessRotation();
							if (cell.ProcessRotation()) {
								redarawIsNeeded = true;
								CSoundBank::Play(CSoundBank::SndClatz);
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

	if (redarawIsNeeded)
		m_Map.DefineConnectStatus();

	return redarawIsNeeded;
}


void CGame::RenderCell(const CTextureBank::TextureType type) const
{
 	//Single cell vertex coordinates
 	static const float CellVertex[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
 
 	glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(type));
 	glVertexPointer(2, GL_FLOAT, 0, CellVertex);
 	glTexCoordPointer(2, GL_SHORT, 0, PlainTex);
 	glDrawElements(GL_TRIANGLES, (sizeof(PlainInd) / sizeof(PlainInd[0])), GL_UNSIGNED_INT, PlainInd);
}


void CGame::OnMouseClick(const Uint8 button)
{
	float posX = 0.0f, posY = 0.0f;
	if (!GetMousePosition(posX, posY))
		return;

	if (button == SDL_BUTTON_LEFT) {
		if (m_Mode == Play) {
			if (m_BtnNext.IsMouseOver(posX, posY)) {
				m_Explosions.clear();
				if (m_NextMapId < 99999999) {
					++m_NextMapId;
					m_Explosions.clear();
					m_Mode = PlayToPlay;
					m_ModeStartTime = CSynchro::GetTick();
				}
			}
			else if (m_BtnPrev.IsMouseOver(posX, posY)) {
				if (m_NextMapId > 1) {
					--m_NextMapId;
					m_Explosions.clear();
					m_Mode = PlayToPlay;
					m_ModeStartTime = CSynchro::GetTick();
				}
			}
			else if (m_BtnReset.IsMouseOver(posX, posY)) {
				m_Explosions.clear();
				m_Map.ResetByRotate();
			}
			else if (m_BtnInfo.IsMouseOver(posX, posY)) {
				m_Mode = PlayToInfo;
				m_ModeStartTime = CSynchro::GetTick();
			}
		}
		else if (m_Mode == Info) {
			if (m_BtnOk.IsMouseOver(posX, posY)) {
				m_Mode = InfoToPlay;
				m_ModeStartTime = CSynchro::GetTick();
			}
		}
	}

	if (m_Mode == Play && !m_Map.IsGameOver() && posX > -5.0f && posX < 5.0f && posY > -5.0f && posY < 5.0f) {	//point in game field
		const float scale = 10.0f / static_cast<float>(m_Map.GetMapSize());
		const unsigned short x = static_cast<unsigned short>((posX + 5.0f) / scale);
		const unsigned short y = m_Map.GetMapSize() - 1 - static_cast<unsigned short>((posY + 5.0f) / scale);
		
		CCell& cell = m_Map.GetCell(x, y);
		if (cell.GetCellType() != CCell::CTFree) {
			switch (button) {
				case SDL_BUTTON_MIDDLE:
					cell.ReverseLock();
					break;
				case SDL_BUTTON_LEFT:
				case SDL_BUTTON_RIGHT:
					cell.Rotate(button != SDL_BUTTON_LEFT);
					m_Map.DefineConnectStatus();
					break;
				default:
					break;
			}
		}
	}		
	PostRedrawEvent();
}


void CGame::RenderInfo()
{

}
