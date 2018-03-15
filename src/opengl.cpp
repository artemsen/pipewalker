/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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

#include "opengl.h"
#include "common.h"


//Window subsystem
#if defined PW_USE_GLUT
	#include "wss_glut.h"
#elif defined PW_SYSTEM_WINNT
	#include "wss_winnt.h"
#elif defined PW_SYSTEM_NIX
	#include "wss_x11.h"
#else
	#error Undefined window subsystem
#endif


//Game properties
static const unsigned short PW_GAME_MODES[3][2] = {
	{10, 25},	///< Game map mode 10 (width) * 10 (heigth) with 25 (receivers) - deafult mode
	{20, 90},	///< Game map mode 20 (width) * 20 (heigth) with 90 (receivers)
	{30, 150}	///< Game map mode 30 (width) * 30 (heigth) with 150 (receivers)
};

struct GameSettings {
	GameSettings() : Size(PW_GAME_MODES[0]), ID(0) {}
	const unsigned short*	Size;
	unsigned int			ID;
} NewGameSettings;


//Screen objects identifiers (Hi word)
#define PW_BUTTONS_ID			0xB0
#define PW_CELL_ID				0xF0
#define PW_CUST_SPINUP_ID		0xA1
#define PW_CUST_SPINDN_ID		0xA2
#define PW_CUST_MAPSIZE_ID		0xA3
//Screen buttons (Lo word)
#define PW_BTN_NEW_ID			0x01
#define PW_BTN_RESET_ID			0x02
#define PW_BTN_INFO_ID			0x03
#define PW_BTN_CUSTOM_ID		0x04
#define PW_BTN_OK_ID			0x05
#define PW_BTN_CANCEL_ID		0x06


COpenGL::COpenGL(void)
: m_enuCurrentMode(ModeInfo),
  m_enuNewMode(ModeInfo),
  m_fMotionFirstPhase(true),
  m_nMotionStartTime(0),
  m_pGameLogic(NULL),
  m_pWinSubsystem(NULL),
  m_pObjects(NULL)
{
}


COpenGL::~COpenGL()
{
	if (m_pGameLogic)
		delete m_pGameLogic;
	if (m_pObjects)
		delete m_pObjects;
}


int COpenGL::Run(void)
{
	COpenGL gl;

	//Set window manager
#if defined PW_USE_GLUT
	CGlut wndMgr(&gl);
#elif defined PW_SYSTEM_WINNT
	CMSWindows wndMgr(&gl);
#elif defined PW_SYSTEM_NIX
	CXServer wndMgr(&gl);
#endif
	if (!wndMgr.Initialize())
		return 1;

	if (!gl.Initialize(&wndMgr))
		return 1;

	return wndMgr.DoMainLoop();
}


bool COpenGL::Initialize(CWinSubsystem* pWinSubsystem)
{
	//Set current window manager
	m_pWinSubsystem = pWinSubsystem;

	//Initialize OpenGL subsystem
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glViewport(0, 0, PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	SetupProjection();
	glMatrixMode(GL_MODELVIEW);

	//Set up lights
	GLfloat LightAmbient[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightSpc[]    	= {-0.2f,-0.2f,-0.2f, 1.0f };
	GLfloat LightPosition[] = {-5.0f, 5.0f, 10.0f, 1.0f };	//Light position
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	//Create fog
	/*
	glEnable(GL_FOG);
	GLfloat fogColor[4] = {0.0f, 0.0f, 0.2f, 0.0f};	//background color
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.2f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, -5.0f);
	*/

	//Create class objects
	m_pGameLogic = new CGame;
	m_pObjects = new CGLObjects;

	//Load textures and create display lists
	if (!m_pObjects->Initialize()) {
		m_pWinSubsystem->ShowErrorMessage(m_pObjects->GetLastError());
		return false;
	}

	//Start new game
	SetNewMode(ModeNewGame);

	return true;
}


void COpenGL::OnMouseClick(const MouseButton enuButton, const int nXCoord, const int nYCoord)
{
	if (m_nMotionStartTime != 0)	//Motion in progress
		return;

	GLint	glViewport[4];
	GLuint	glBuffer[512];

	glGetIntegerv(GL_VIEWPORT, glViewport);
	glSelectBuffer(sizeof(glBuffer), glBuffer);

	glRenderMode(GL_SELECT);
	glInitNames();	//Initializes the name stack
	glPushName(0);	//Push 0 (at least one entry) into the stack

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		PickMatrix(static_cast<GLfloat>(nXCoord), static_cast<GLfloat>(glViewport[3] - nYCoord), 0.001f, 0.001f, glViewport);
		SetupProjection();
		glMatrixMode(GL_MODELVIEW);
		//Render to get objects names
		DrawEnvironment();
		if (m_enuCurrentMode == ModePlayGame)
			DrawGame(GL_SELECT);
		else if (m_enuCurrentMode == ModeCustomGame)
			DrawSetUpGame(GL_SELECT);
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	GLint nSelNum = glRenderMode(GL_RENDER);

	if (nSelNum > 1) {
		unsigned int nIDChoose = glBuffer[((nSelNum - 1) * 4) + 3];
#ifndef NDEBUG
		printf("Processing ID 0x%08x\n", nIDChoose);
#endif	//NDEBUG


		if (HIWORD(nIDChoose) == PW_BUTTONS_ID) {
			//Screen button (environment) click
			switch (LOWORD(nIDChoose)) {
				case PW_BTN_NEW_ID:
					SetNewMode(ModeNewGame);
					break;
				case PW_BTN_RESET_ID:
					ResetGame();
					break;
				case PW_BTN_INFO_ID:
					SetNewMode(ModeInfo);
					break;
				case PW_BTN_CUSTOM_ID:
					SetNewMode(ModeCustomGame);
					break;
				case PW_BTN_OK_ID:
					SetNewMode(m_enuCurrentMode == ModeCustomGame ? ModeNewGame : ModePlayGame);
					break;
				case PW_BTN_CANCEL_ID:
					SetNewMode(ModePlayGame);
					break;
				default:
					assert(NULL && "Undefined button identifier");
					break;
			}
		}
		else {
			bool fRedisplay = false;
			switch (m_enuCurrentMode) {
				case ModePlayGame:
					assert(HIWORD(nIDChoose) == PW_CELL_ID);
					if (!m_pGameLogic->IsGameOver()) {
						#ifndef NDEBUG
						printf("Processing cell %02i x %02i\n", LOBYTE(LOWORD(nIDChoose)), HIBYTE(LOWORD(nIDChoose)));
						#endif	//NDEBUG
						CCell* pCell = m_pGameLogic->GetObject(LOBYTE(LOWORD(nIDChoose)), HIBYTE(LOWORD(nIDChoose)));
						if (pCell->Type != CCell::Free) {
							if (enuButton == MiddleButton)
								pCell->ReverseLock();
							else
								m_pGameLogic->RotateTube(pCell, enuButton == RightButton ? CCell::Positive : CCell::Negative);
							fRedisplay = true;
						}
					}
					break;
				case ModeInfo:
				case ModeNewGame:
					//Nothing - it must be "ureachable code"
					break;
				case ModeCustomGame:
					if (HIWORD(nIDChoose) == PW_CUST_MAPSIZE_ID)
						NewGameSettings.Size = PW_GAME_MODES[LOWORD(nIDChoose)];
					else {
						assert(HIWORD(nIDChoose) == PW_CUST_SPINUP_ID || HIWORD(nIDChoose) == PW_CUST_SPINDN_ID);
						//unsigned short nMapIdNum = (m_nMapId & (0xF0000000 >> (HIBYTE(nChooseID) * sizeof(unsigned int)))) >> (sizeof(unsigned int) * 8 - sizeof(unsigned int) - HIBYTE(nChooseID) * sizeof(unsigned int));
						unsigned int nDelta = 0x10000000 >> (LOWORD(nIDChoose) * sizeof(unsigned int));
						if (HIWORD(nIDChoose) == PW_CUST_SPINUP_ID)
							NewGameSettings.ID += nDelta;
						else
							NewGameSettings.ID -= nDelta;
					}
					fRedisplay = true;
					break;
			}
			if (fRedisplay)
				m_pWinSubsystem->PostRedisplay();
		}
    }
}


void COpenGL::OnKeyPress(const int nKeyCode)
{
	if (m_nMotionStartTime != 0)	//Motion in progress
		return;

	switch (nKeyCode) {
		case 27:	//ASCII code for the escape key
			if (m_enuCurrentMode == ModePlayGame)
				m_pWinSubsystem->PostExit();
			else
				SetNewMode(ModePlayGame);
			break;
		case 'R':
		case 'r':	//Reset current map
			if (m_enuCurrentMode == ModePlayGame)
				ResetGame();
			break;
		case 'N':
		case 'n':	//Create new map
			SetNewMode(ModeNewGame);
			break;
		case 'H':
		case 'h':	//Help
			if (m_enuCurrentMode == ModePlayGame)
				SetNewMode(ModeInfo);
			break;
	}
}


void COpenGL::OnDraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	bool fRedisplay = false;


	glPushMatrix();
		if (m_nMotionStartTime != 0) {
			//Motion in progress
			GLfloat dMotionAngle = 0.0f;
			unsigned long nCurrTime = GetTickCount();
			unsigned long nDiffTime = nCurrTime - m_nMotionStartTime;
			if (nDiffTime < PW_ROTATION_TIME * 1.5f) {
				dMotionAngle = -(static_cast<float>(nDiffTime) / static_cast<float>(PW_ROTATION_TIME * 1.5f)) * PW_ROTATION_ANGLE;
				if (!m_fMotionFirstPhase)
					dMotionAngle += PW_ROTATION_ANGLE;
			}
			else {
				//Next phase finished
				if (m_fMotionFirstPhase) {
					if (m_enuNewMode == ModeNewGame) {
						//Show waiting message and generate new level
						m_pGameLogic->New(NewGameSettings.Size[0], NewGameSettings.Size[1], NewGameSettings.ID);
						NewGameSettings.ID = 0;
						m_enuNewMode = ModePlayGame;
					}
					else {
						m_fMotionFirstPhase = false;
						m_enuCurrentMode = m_enuNewMode;
						m_nMotionStartTime = GetTickCount();
					}
					dMotionAngle = PW_ROTATION_ANGLE;
				}
				else {
					m_nMotionStartTime = 0;	//Motion finished
				}
			}
			glRotatef(dMotionAngle, 0.0f, 1.0f, 0.0f);
			fRedisplay = true;
		}

		switch (m_enuCurrentMode) {
			case ModePlayGame:
				fRedisplay |= DrawGame(GL_RENDER);
				break;
			case ModeInfo:
				m_pObjects->BindTexture(CGLObjects::TxrEnvironment);
				glBegin(GL_QUADS);
					glNormal3f(0.0f, 0.0f, 1.0f);
					glTexCoord2f(0.0f, 5.0f); glVertex3f(-5.0f, 5.0f, 0.0f);
					glTexCoord2f(5.0f, 5.0f); glVertex3f( 5.0f, 5.0f, 0.0f);
					glTexCoord2f(5.0f, 0.0f); glVertex3f( 5.0f,-5.0f, 0.0f);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f,-5.0f, 0.0f);
				glEnd();
				m_pObjects->BindTexture(CGLObjects::TxrWndInfo);
				glBegin(GL_QUADS);
					glNormal3f(0.0f, 0.0f, 1.0f);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f, 5.0f, 0.0f);
					glTexCoord2f(1.0f, 1.0f); glVertex3f( 5.0f, 5.0f, 0.0f);
					glTexCoord2f(1.0f, 0.0f); glVertex3f( 5.0f,-5.0f, 0.0f);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f,-5.0f, 0.0f);
				glEnd();
				break;
			case ModeCustomGame:
				DrawSetUpGame(GL_RENDER);
				break;
			case ModeNewGame:
				break;
		}
	glPopMatrix();

	//Draw environment
	DrawEnvironment();

	//Draw map ID
	if (m_nMotionStartTime == 0 && m_enuCurrentMode == ModePlayGame) {
		glPushMatrix();
			glTranslatef(-2.4f, -5.3f, 0.0f);
			m_pObjects->DrawStatusBar(m_pGameLogic->GetMapID());
		glPopMatrix();
	}

	if (fRedisplay)
		m_pWinSubsystem->PostRedisplay();
}


void COpenGL::DrawEnvironment(void)
{
	//Draw environment
	m_pObjects->DrawObject(CGLObjects::ObjEnvironment);

	if (m_nMotionStartTime != 0)
		return;	//Don't draw buttons in motion mode

	//Draw buttons
	glPushMatrix();
		glTranslatef(-5.0f, -6.5f, 0.0f);
		switch (m_enuCurrentMode) {
			case ModePlayGame:
				//New button
				glLoadName(MAKELONG(PW_BTN_NEW_ID, PW_BUTTONS_ID));
				m_pObjects->DrawButton(CGLObjects::TxrButtonNew);

				//Reset button
				glLoadName(MAKELONG(PW_BTN_RESET_ID, PW_BUTTONS_ID));
				glTranslatef(1.5f, 0.0f, 0.0f);
				m_pObjects->DrawButton(CGLObjects::TxrButtonRset);

				//Custom button
				glLoadName(MAKELONG(PW_BTN_CUSTOM_ID, PW_BUTTONS_ID));
				glTranslatef(5.9f, 0.0f, 0.0f);
				m_pObjects->DrawButton(CGLObjects::TxrButtonCust);

				//Info button
				glLoadName(MAKELONG(PW_BTN_INFO_ID, PW_BUTTONS_ID));
				glTranslatef(1.5f, 0.0f, 0.0f);
				m_pObjects->DrawButton(CGLObjects::TxrButtonInfo);
				break;
			case ModeInfo:
			case ModeNewGame:
			case ModeCustomGame:
				break;
		}
	glPopMatrix();

	if (m_enuCurrentMode == ModeCustomGame || m_enuCurrentMode == ModeInfo) {
		//OK button
		glPushMatrix();
			glLoadName(MAKELONG(PW_BTN_OK_ID, PW_BUTTONS_ID));
			glTranslatef(4.0f, -6.5f, 0.0f);
			m_pObjects->DrawButton(CGLObjects::TxrButtonOk);
		glPopMatrix();
	}

	if (m_enuCurrentMode == ModeCustomGame) {
		//Cancel button
		glLoadName(MAKELONG(PW_BTN_CANCEL_ID, PW_BUTTONS_ID));
		glPushMatrix();
			glTranslatef(-5.0f, -6.5f, 0.0f);
			m_pObjects->DrawButton(CGLObjects::TxrButtonCncl);
		glPopMatrix();
	}
}


void COpenGL::DrawSetUpGame(int nGLMode)
{
	if (nGLMode != GL_SELECT) {
		m_pObjects->BindTexture(CGLObjects::TxrEnvironment);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 5.0f); glVertex3f(-5.0f, 5.0f, 0.0f);
			glTexCoord2f(5.0f, 5.0f); glVertex3f( 5.0f, 5.0f, 0.0f);
			glTexCoord2f(5.0f, 0.0f); glVertex3f( 5.0f,-5.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f,-5.0f, 0.0f);
		glEnd();

		m_pObjects->BindTexture(CGLObjects::TxrWndCustom);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f, 5.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 5.0f, 5.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 5.0f,-5.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f,-5.0f, 0.0f);
		glEnd();
	}

	int i;	//Counter

	//Draw radio buttons
	for (i = 0; i < 3; i++) {
		glLoadName(MAKELONG(i, PW_CUST_MAPSIZE_ID));
		glPushMatrix();
			glTranslatef(-4.4f, 2.7f - i * 1.4f, 0.1f);
			m_pObjects->DrawRadioButton(NewGameSettings.Size == PW_GAME_MODES[i]);
		glPopMatrix();
	}

	//Draw map id choose
	for (i = 0; i < 8; i++) {
		glPushMatrix();
			glTranslatef(-1.7f + i / 2.0f, -2.8f, 0.1f);

			//Top spin
			glLoadName(MAKELONG(i, PW_CUST_SPINUP_ID));
			m_pObjects->DrawObject(CGLObjects::ObjSpin);

			//Map id
			glTranslatef(-0.2f, -0.6f, 0.0f);
			unsigned short nMapIdNum = static_cast<unsigned short>((NewGameSettings.ID & (0xF0000000 >> (i * sizeof(unsigned int)))) >> (sizeof(unsigned int) * 8 - sizeof(unsigned int) - i * sizeof(unsigned int)));
			m_pObjects->PrintHexNumber(nMapIdNum);

			//Bottom spin
			glLoadName(MAKELONG(i, PW_CUST_SPINDN_ID));
			glTranslatef(0.2f, -1.1f, 0.0f);
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			m_pObjects->DrawObject(CGLObjects::ObjSpin);
		glPopMatrix();
	}
}


bool COpenGL::DrawGame(int nGLMode)
{
	bool fRedisplay = false;

	DrawMapCells();

	if (nGLMode != GL_SELECT) {
		//Draw objects shadow
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPushMatrix();
			glTranslatef(0.05f, -0.05f, 0.0f);
			glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
			fRedisplay |= DrawMapObjects(true);
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);

		//Draw objects
		fRedisplay |= DrawMapObjects(false);
	}

	return fRedisplay;
}


void COpenGL::DrawMapCells(void)
{
	glPushMatrix();
	//Set scale
	GLfloat glScale = 10.0f / m_pGameLogic->GetMapSize();	//10.0f is a usual size
	glScalef(glScale, glScale, glScale);

	//Draw cells background
	for (unsigned short nY = 0; nY < m_pGameLogic->GetMapSize(); nY++) {
		GLfloat dY = -(m_pGameLogic->GetMapSize() / 2.0f) + 0.5f + nY;
		for (unsigned short nX = 0; nX < m_pGameLogic->GetMapSize(); nX++) {
			GLfloat dX = -(m_pGameLogic->GetMapSize() / 2.0f) + 0.5f + nX;
			glLoadName(MAKELONG(MAKEWORD(nX, nY), PW_CELL_ID));	//Set current coordinates by cell on map as name
			glPushMatrix();
				glTranslatef(dX, -dY, 0.0f);
				m_pObjects->DrawObject(CGLObjects::ObjCell);
			glPopMatrix();
		}
	}
	glPopMatrix();
}


bool COpenGL::DrawMapObjects(bool fIsShadow)
{
	bool fRedisplay = false;

	glPushMatrix();
	//Set scale
	GLfloat glScale = 10.0f / m_pGameLogic->GetMapSize();	//10.0f is a usual size
	glScalef(glScale, glScale, glScale);

	for (unsigned short nY = 0; nY < m_pGameLogic->GetMapSize(); nY++) {
		GLfloat dY = -(m_pGameLogic->GetMapSize() / 2.0f) + 0.5f + nY;
		for (unsigned short nX = 0; nX < m_pGameLogic->GetMapSize(); nX++) {
			GLfloat dX = -(m_pGameLogic->GetMapSize() / 2.0f) + 0.5f + nX;
			CCell* pCell = m_pGameLogic->GetObject(nX, nY);
			glPushMatrix();
				glTranslatef(dX, -dY, 0.0f);
				//Draw object
				switch (pCell->Type) {
					case CCell::Free:
						break;	//Free cell
					case CCell::Tube:
						fRedisplay |= DrawTube(pCell, fIsShadow);
						break;
					case CCell::Receiver:
						glPushMatrix();
						fRedisplay |= DrawTube(pCell, fIsShadow);
						glPopMatrix();
						//Draw system block
						m_pObjects->DrawObject(CGLObjects::ObjReceiverSb);
						if (m_pGameLogic->IsGameOver()) {
							//Make user happy!
							static GLfloat dAngle = 1.0f;
							static GLfloat dDir = 0.005f;
							if (dAngle > 5.0f || dAngle < -5.0f)
								dDir = -dDir;
							dAngle += dDir;
							glRotatef(dAngle, 0.0f, 0.0f, 1.0f);
							glTranslatef(-dAngle / 100.0f, 0.0f, 0.0f);
							fRedisplay = true;
						}
						//Draw monitor
						m_pObjects->BindTexture(pCell->State ? CGLObjects::TxrRcvActive : CGLObjects::TxrRcvPassive);
						m_pObjects->DrawObject(CGLObjects::ObjReceiverMon);
						break;
					case CCell::Sender:
						glPushMatrix();
						fRedisplay |= DrawTube(pCell, fIsShadow);
						glPopMatrix();
						m_pObjects->DrawObject(CGLObjects::ObjSender);
						break;
					default:
						assert(false && "Unknow object type");
						break;
				}
				//Draw lock
				if (!fIsShadow && pCell->IsLocked()) {
					glDisable(GL_DEPTH_TEST);
						m_pObjects->DrawObject(CGLObjects::ObjLock);
					glEnable(GL_DEPTH_TEST);
				}
			glPopMatrix();
		}
	}
	glPopMatrix();
	return fRedisplay;
}


bool COpenGL::DrawTube(CCell* pCell, bool fIsShadow)
{
	bool fRedisplay = false;

	//Rotation the tube
	if (pCell->IsRotationInProgress()) {
		if (fIsShadow) {	//Calculate new angle
			//Rotation in progress
			unsigned long nCurrTime = GetTickCount();
			unsigned long nDiffTime = nCurrTime - pCell->Rotate.StartTime;
			if (nDiffTime < PW_ROTATION_TIME) {
				GLfloat dDegree = (static_cast<float>(nDiffTime) / static_cast<float>(PW_ROTATION_TIME)) * PW_ROTATION_ANGLE;
				pCell->Rotate.Angle = (pCell->Rotate.Dir == CCell::Negative ? dDegree : -dDegree);
			}
			else
				m_pGameLogic->OnTubeRotated(pCell);
		}
		glRotatef(pCell->Rotate.Angle, 0.0f, 0.0f, 1.0f);
		fRedisplay = true;
	}

	if (!fIsShadow)
		m_pObjects->BindTexture(pCell->State || pCell->Type == CCell::Sender ? CGLObjects::TxrActiveTube : CGLObjects::TxrPassiveTube);

	//Draw tubes
	if (pCell->IsCurved())
		m_pObjects->DrawObject(CGLObjects::ObjTubeJoiner);
	if (pCell->Sides & CONNECT_UP)
		m_pObjects->DrawObject(CGLObjects::ObjTube);

	if (pCell->Sides & CONNECT_DOWN) {
		glPushMatrix();
			glRotatef(PW_ROTATION_ANGLE * 2.0f, 0.0f, 0.0f, 1.0f);
			m_pObjects->DrawObject(CGLObjects::ObjTube);
		glPopMatrix();
	}

	if (pCell->Sides & CONNECT_RIGHT) {
		glPushMatrix();
			glRotatef(-PW_ROTATION_ANGLE, 0.0f, 0.0f, 1.0f);
			m_pObjects->DrawObject(CGLObjects::ObjTube);
		glPopMatrix();
	}

	if (pCell->Sides & CONNECT_LEFT) {
		glPushMatrix();
			glRotatef(PW_ROTATION_ANGLE, 0.0f, 0.0f, 1.0f);
			m_pObjects->DrawObject(CGLObjects::ObjTube);
		glPopMatrix();
	}

	return fRedisplay;
}


void COpenGL::SetNewMode(const Mode enuNewMode)
{
	m_enuNewMode = enuNewMode;
	if (m_enuNewMode == ModeCustomGame) {
		NewGameSettings.ID = m_pGameLogic->GetMapID();
	}
	m_fMotionFirstPhase = true;
	m_nMotionStartTime = GetTickCount();
	m_pWinSubsystem->PostRedisplay();
}


void COpenGL::LookAt(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat lookAtX, GLfloat lookAtY, GLfloat lookAtZ, GLfloat upX, GLfloat upY, GLfloat upZ) const
{
	//Vector
	GLfloat f[3];
	
	//Calculating the viewing vector
	f[0] = lookAtX - eyeX;
	f[1] = lookAtY - eyeY;
	f[2] = lookAtZ - eyeZ;
	
	GLfloat dMag = static_cast<GLfloat>(sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]));
	GLfloat upMag = static_cast<GLfloat>(sqrt(upX * upX + upY * upY + upZ * upZ));
	
	//Normalizing the viewing vector
	if (dMag != 0) {
		f[0] = f[0] / dMag;
		f[1] = f[1] / dMag;
		f[2] = f[2] / dMag;
	}
	
	//Normalising the up vector. no need for this here if you have your
	//   up vector already normalised, which is mostly the case
	if (upMag != 0) {
		upX = upX / upMag;
		upY = upY / upMag;
		upZ = upZ / upMag;
	}
	
	GLfloat s[3], u[3];
	
	CrossProd(f[0], f[1], f[2], upX, upY, upZ, s);
	CrossProd(s[0], s[1], s[2], f[0], f[1], f[2], u);
	
	GLfloat M[] = {
		s[0], u[0], -f[0], 0,
		s[1], u[1], -f[1], 0,
		s[2], u[2], -f[2], 0,
		0,    0,     0,    1
	};
	
	glMultMatrixf(M);
	glTranslatef (-eyeX, -eyeY, -eyeZ);
}


void COpenGL::PickMatrix(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const GLint viewport[4]) const
{
	GLfloat m[16];
	GLfloat sx, sy;
	GLfloat tx, ty;
	
	sx = viewport[2] / width;
	sy = viewport[3] / height;
	tx = (viewport[2] + 2.0f * (viewport[0] - x)) / width;
	ty = (viewport[3] + 2.0f * (viewport[1] - y)) / height;
	
#define M(row, col) m[col * 4 + row]
	M(0, 0) = sx;   M(0, 1) = 0.0f; M(0, 2) = 0.0f; M(0, 3) = tx;
	M(1, 0) = 0.0f; M(1, 1) = sy;   M(1, 2) = 0.0f; M(1, 3) = ty;
	M(2, 0) = 0.0f; M(2, 1) = 0.0f; M(2, 2) = 1.0f; M(2, 3) = 0.0f;
	M(3, 0) = 0.0f; M(3, 1) = 0.0f; M(3, 2) = 0.0f; M(3, 3) = 1.0f;
#undef M
	
	glMultMatrixf(m);
}


void COpenGL::CrossProd(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat res[3]) const
{
	res[0] = y1 * z2 - y2 * z1;
	res[1] = x2 * z1 - x1 * z2;
	res[2] = x1 * y2 - x2 * y1;
}


void COpenGL::SetupProjection() const
{
	const GLfloat dAspect = 0.75f;	// 3 x 4 sides aspect
	const GLfloat dFov = 50.0f;
    const GLfloat dNear = 1.0f;
	GLfloat dTop = static_cast<GLfloat>(tan(dFov * 3.14159f / 360.0f) * dNear);
    GLfloat dBottom = -dTop;
    GLfloat dLeft = dAspect * dBottom;
    GLfloat dRight = dAspect * dTop;

	glFrustum(dLeft, dRight, dBottom, dTop, dNear, 20.0f);

	LookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}
	

void COpenGL::OnWndSizeChanged(const int nWidth, const int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);
	SetupProjection();
}
