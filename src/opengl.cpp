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
#include "texload.h"

//Window subsystem
#ifdef PWTARGET_WINNT
#include "mswindows.h"
#else
#include "xserver.h"
#endif

//Screen buttons identifiers
#define ID_BUTTON_NEW	0xBBBB0001
#define ID_BUTTON_RESET	0xBBBB0002


COpenGL::COpenGL(void)
: m_pWinSubsystem(0)
{
}


COpenGL::~COpenGL(void)
{
}


int COpenGL::Run(void)
{
	COpenGL gl;

	//Set window manager
#ifdef PWTARGET_WINNT
	CMSWindows wndMgr(&gl);
#else
	CXServer wndMgr(&gl);
#endif
	if (!wndMgr.Initialize())
		return 1;

	gl.SetWndSubsystem(&wndMgr);

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

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, static_cast<GLfloat>(SCREEN_WIDTH) / static_cast<GLfloat>(SCREEN_HEIGHT), 1.0f, 20.0f);
	gluLookAt(0,0,15,0,0,-10,0,1,0);
	glMatrixMode(GL_MODELVIEW);

	//Set up lights
	GLfloat LightAmbient[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightSpc[]    	= {-0.2f,-0.2f,-0.2f, 1.0f };
	GLfloat LightPosition[] = {-3.0f, 3.0f,10.0f, 1.0f };	//Light position
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	//Load the textures from resources
#ifdef PWTARGET_WINNT
	if (!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrCell],			IDTEX_CELLBKGR) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrEnvironment],	IDTEX_BKGR) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrActiveTube],	IDTEX_TUBEACT) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrPassiveTube],	IDTEX_TUBEPSV) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrSender],		IDTEX_SENDER) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrButtonNew],		IDTEX_BTNNEW) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrButtonRst],		IDTEX_BTNRSET) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrWebLink],		IDTEX_WEBLINK) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrTitle],			IDTEX_TITLE) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrWinTitle],		IDTEX_CONGR) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvActive],		IDTEX_DISPACT) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvPassive],	IDTEX_DISPPSV) ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvBack],		IDTEX_RECV))
		return 1;
#else
	//Load the textures from file
	if (!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrCell],			"textures/cellbkg.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrEnvironment],	"textures/bkgr.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrActiveTube],	"textures/tube_act.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrPassiveTube],	"textures/tube_psv.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrSender],		"textures/sender.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrButtonNew],		"textures/btn_new.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrButtonRst],		"textures/btn_reset.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrWebLink],		"textures/weblink.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrTitle],			"textures/title.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrWinTitle],		"textures/congr.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvActive],		"textures/display_act.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvPassive],	"textures/display_psv.tga") ||
		!CTextureLoader::LoadTexture(&gl.m_aTextures[TxrRcvBack],		"textures/recv.tga"))
		return 1;
#endif

	//Start new game
	gl.NewGame();

	return wndMgr.DoMainLoop();
}


void COpenGL::SetWndSubsystem(CWinSubsystem* pWinSubsystem)
{
	//Set current window manager
	m_pWinSubsystem = pWinSubsystem;
}


void COpenGL::OnMouseClick(const MouseButton enuButton, const int nXCoord, const int nYCoord)
{
	GLint	glViewport[4];
	GLuint	glBuffer[512];

	glGetIntegerv(GL_VIEWPORT, glViewport);
	glSelectBuffer(sizeof(glBuffer), glBuffer);

	glRenderMode(GL_SELECT);
	glInitNames();	//Initializes the name stack
	glPushName(0);	//Push 0 (at least one entry) onto the stack

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		gluPickMatrix(static_cast<GLdouble>(nXCoord), static_cast<GLdouble>(glViewport[3]) - nYCoord, 1.0f, 1.0f, glViewport);
		gluPerspective(45.0f, static_cast<GLfloat>(SCREEN_WIDTH) / static_cast<GLfloat>(SCREEN_HEIGHT), 1.0f, 20.0f);
		gluLookAt(0,0,15,0,0,-10,0,1,0);
		glMatrixMode(GL_MODELVIEW);
		RenderEnv();					//Render to get objects names
		RenderBackground();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	GLint nSelNum = glRenderMode(GL_RENDER);

	if (nSelNum > 1) {
		unsigned int nIDChoose = glBuffer[((nSelNum - 1) * 4) + 3];
		if (nIDChoose == ID_BUTTON_NEW) {
			NewGame();
		}
		else if (nIDChoose == ID_BUTTON_RESET) {
			ResetGame();
		}
		else if (!m_objGame.IsGameOver()) {
			CCell* pObj = m_objGame.GetObject(LOBYTE(nIDChoose), HIBYTE(nIDChoose));
			pObj->StartRotate(enuButton != RightButton, enuButton == MiddleButton);
			m_pWinSubsystem->PostRedisplay();
		}
    }
}

void COpenGL::OnKeyPress(const int nKeyCode)
{
	switch (nKeyCode) {
		case 27:	//ASCII code for the escape key
			m_pWinSubsystem->PostExit();
			break;
		case 'R':
		case 'r':	//Reset current map
			ResetGame();
			break;
		case 'N':
		case 'n':	//Create new map
			NewGame();
			break;
	}
}


void COpenGL::OnDraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//Draw background
	RenderBackground();

	//Draw shadow
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0.05f, -0.05f, 0.0f);
		glColor3f(0.3f, 0.3f, 0.3f);
		RenderScene(true);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	//Draw scene
	RenderScene(false);

	//Draw environment
	RenderEnv();

	if (m_objGame.IsGameOver()) {
		// Game is over... Draw win title
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrWinTitle]);
		glBegin(GL_QUADS);
			glNormal3b(0, 0, 1);
			glTexCoord2i(0, 1); glVertex3f(-2.8f, -5.3f, -1.0f);
			glTexCoord2i(1, 1); glVertex3f( 2.8f, -5.3f, -1.0f);
			glTexCoord2i(1, 0); glVertex3f( 2.8f, -6.0f, -1.0f);
			glTexCoord2i(0, 0); glVertex3f(-2.8f, -6.0f, -1.0f);
		glEnd();

#ifdef PWTARGET_WINNT
		Sleep(50);
#endif
		//wait(50)
		m_pWinSubsystem->PostRedisplay();
	}
}

void COpenGL::RenderEnv(void)
{
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrEnvironment]);

	//Main field sides
	glBegin(GL_QUADS);
		glNormal3b(-1, 0, 0);
		glTexCoord2i(0, 0); glVertex3f(5.0f, -5.0f, -1.2f);
		glTexCoord2i(0, 5);	glVertex3f(5.0f,  5.0f, -1.2f);
		glTexCoord2i(1, 5); glVertex3f(5.0f,  5.0f, -0.5f);
		glTexCoord2i(1, 0); glVertex3f(5.0f, -5.0f, -0.5f);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3b(1, 0, 0);
		glTexCoord2i(0, 0); glVertex3f(-5.0f, -5.0f, -1.2f);
		glTexCoord2i(0, 5); glVertex3f(-5.0f,  5.0f, -1.2f);
		glTexCoord2i(1, 5); glVertex3f(-5.0f,  5.0f, -0.5f);
		glTexCoord2i(1, 0); glVertex3f(-5.0f, -5.0f, -0.5f);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3b(0, -1, 0);
		glTexCoord2i(0, 0); glVertex3f(-5.0f, 5.0f, -1.2f);
		glTexCoord2i(0, 1); glVertex3f(-5.0f, 5.0f, -0.5f);
		glTexCoord2i(5, 1); glVertex3f( 5.0f, 5.0f, -0.5f);
		glTexCoord2i(5, 0); glVertex3f( 5.0f, 5.0f, -1.2f);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3b(0, 1, 0);
		glTexCoord2i(0, 0); glVertex3f(-5.0f, -5.0f, -1.2f);
		glTexCoord2i(0, 1); glVertex3f(-5.0f, -5.0f, -0.5f);
		glTexCoord2i(5, 1); glVertex3f( 5.0f, -5.0f, -0.5f);
		glTexCoord2i(5, 0); glVertex3f( 5.0f, -5.0f, -1.2f);
	glEnd();

	//Top front side
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 0); glVertex3f( 9.0f,  9.0f, -2.5f);
		glTexCoord2i(0, 2); glVertex3f(-9.0f,  9.0f, -2.5f);
		glTexCoord2i(3, 2); glVertex3f(-9.0f, -9.0f, -2.5f);
		glTexCoord2i(3, 0); glVertex3f( 9.0f, -9.0f, -2.5f);
	glEnd();

	//Title
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrTitle]);
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(-3.0f, 6.3f, -1.0f);
		glTexCoord2i(1, 1); glVertex3f( 3.0f, 6.3f, -1.0f);
		glTexCoord2i(1, 0); glVertex3f( 3.0f, 5.4f, -1.0f);
		glTexCoord2i(0, 0); glVertex3f(-3.0f, 5.4f, -1.0f);
	glEnd();

	//Web link
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrWebLink]);
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(-3.0f, -6.1f, -1.0f);
		glTexCoord2i(1, 1); glVertex3f( 3.0f, -6.1f, -1.0f);
		glTexCoord2i(1, 0); glVertex3f( 3.0f, -6.4f, -1.0f);
		glTexCoord2i(0, 0); glVertex3f(-3.0f, -6.4f, -1.0f);
	glEnd();

	//Button properties
	const GLfloat dBtnWitdh = 3.0f;
	const GLfloat dBtnHeight = 0.8f;
	GLfloat dBtnX, dBtnY;

	//New button
	glLoadName(ID_BUTTON_NEW);
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrButtonNew]);
	dBtnX = -5.0f;
	dBtnY = -5.3f;
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(dBtnX,             dBtnY,              -1.0f);
		glTexCoord2i(1, 1); glVertex3f(dBtnX + dBtnWitdh, dBtnY,              -1.0f);
		glTexCoord2i(1, 0); glVertex3f(dBtnX + dBtnWitdh, dBtnY - dBtnHeight, -1.0f);
		glTexCoord2i(0, 0); glVertex3f(dBtnX,             dBtnY - dBtnHeight, -1.0f);
	glEnd();

	//Reset button
	glLoadName(ID_BUTTON_RESET);
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrButtonRst]);
	dBtnX = 2.0f;
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(dBtnX,             dBtnY,              -1.0f);
		glTexCoord2i(1, 1); glVertex3f(dBtnX + dBtnWitdh, dBtnY,              -1.0f);
		glTexCoord2i(1, 0); glVertex3f(dBtnX + dBtnWitdh, dBtnY - dBtnHeight, -1.0f);
		glTexCoord2i(0, 0); glVertex3f(dBtnX,             dBtnY - dBtnHeight, -1.0f);
	glEnd();
}


void COpenGL::RenderBackground(void)
{
	for (int nY = 0; nY < MAP_HEIGHT_NUM; nY++) {
		GLfloat dY = -4.5f + nY;
		for (int nX = 0; nX < MAP_WIDTH_NUM; nX++) {
			GLfloat dX = -4.5f + nX;
			glLoadName(MAKEWORD(nX, nY));
			glPushMatrix();
				glTranslatef(dX, dY, -1.0f);
				glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrCell]);
				glBegin(GL_QUADS);
					glNormal3b(0, 0, 1);
					glTexCoord2i(0, 0); glVertex3f( 0.5f, 0.5f, -0.2f);
					glTexCoord2i(0, 1); glVertex3f(-0.5f, 0.5f, -0.2f);
					glTexCoord2i(1, 1); glVertex3f(-0.5f,-0.5f, -0.2f);
					glTexCoord2i(1, 0); glVertex3f( 0.5f,-0.5f, -0.2f);
				glEnd();
			glPopMatrix();
		}
	}
}


void COpenGL::RenderScene(bool fIsShadow)
{
	bool fRedisplay = false;

	for (int nY = 0; nY < MAP_HEIGHT_NUM; nY++) {
		GLfloat dY = -4.5f + nY;
		for (int nX = 0; nX < MAP_WIDTH_NUM; nX++) {
			GLfloat dX = -4.5f + nX;
			CCell* pCell = m_objGame.GetObject(nX, nY);
			glPushMatrix();
				glTranslatef(dX, dY, -1.0f);
				//Draw object
				switch (pCell->Type) {
					case CCell::ObjNone:
						break;	//Free cell
					case CCell::ObjStrTube:
					case CCell::ObjCurTube:
					case CCell::ObjTubeJoint:
						fRedisplay |= DrawTube(pCell, fIsShadow);
						break;
					case CCell::ObjReceiver:
						glPushMatrix();
						fRedisplay |= DrawTube(pCell, fIsShadow);	//Half tube
						glPopMatrix();
						DrawReciever(pCell);
						break;
					case CCell::ObjSender:
						glPushMatrix();
						fRedisplay |= DrawTube(pCell, fIsShadow);	//Half tube
						glPopMatrix();
						DrawSender(pCell);
						break;
					default:
						assert(false && "Unknow object type");
						break;
				}
			glPopMatrix();
		}
	}
	if (fRedisplay)
		m_pWinSubsystem->PostRedisplay();
}


bool COpenGL::DrawTube(CCell* pCell, bool fIsShadow)
{
	bool fRedisplay = false;

	//Rotation the tube
	GLfloat glTubeAngle = pCell->RotateAngle;
	if (pCell->RotateStart != 0) {
		if (fIsShadow) {
			//Rotation in progress
			unsigned long nCurrTime = GetTickCount();
			unsigned long nDiffTime = nCurrTime - pCell->RotateStart;
			const unsigned long nRotationTime = 300;	//We have to rotate the tube by 300 ms on angle of 90 degree
			if (nDiffTime < nRotationTime) {
				GLfloat glDeg = (static_cast<GLfloat>(nDiffTime) / static_cast<GLfloat>(nRotationTime)) * 90.0f;
				glTubeAngle += pCell->RotateAngleEnd > pCell->RotateAngle ? glDeg : -glDeg;
			}
			else {
				if (!m_objGame.OnTubeRotated(pCell))
					glTubeAngle = pCell->RotateAngleEnd;
				else
					glTubeAngle = pCell->RotateAngle;
				m_objGame.CheckGameOver();
			}
			pCell->RotateCurr = glTubeAngle;
		}
		else {
			glTubeAngle = pCell->RotateCurr;
		}
		fRedisplay = true;
	}

	glRotatef(glTubeAngle, 0.0f, 0.0f, 1.0f);

	//Set usual position for tube
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);

	GLUquadric* pQuadric = gluNewQuadric();
	gluQuadricNormals(pQuadric, GLU_SMOOTH);
	gluQuadricTexture(pQuadric, GL_TRUE);

	if (!fIsShadow) {
		glBindTexture(GL_TEXTURE_2D, pCell->State || pCell->Type == CCell::ObjSender ? m_aTextures[TxrActiveTube] : m_aTextures[TxrPassiveTube]);
	}

	if (pCell->Type == CCell::ObjReceiver || pCell->Type == CCell::ObjSender) {
		//Half tube
		gluCylinder(pQuadric, 0.1f, 0.1f, 0.5f, 8, 8);
	}
	else {

		glTranslatef(0.0f, 0.0f, -0.5f);

		if (pCell->Type == CCell::ObjStrTube || pCell->Type == CCell::ObjTubeJoint) {
			gluCylinder(pQuadric, 0.1f, 0.1f, 1.0f, 8, 8);
		}

		if (pCell->Type == CCell::ObjTubeJoint) {
			glTranslatef(0.0f, 0.5f, 0.5f);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			gluCylinder(pQuadric, 0.1f, 0.1f, 0.5f, 8, 8);
		}
		if (pCell->Type == CCell::ObjCurTube) {
			glTranslatef(0.0f, 0.5f, 0.5f);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			gluCylinder(pQuadric, 0.1f, 0.1f, 0.55f, 8, 8);
			glTranslatef(0.0f, 0.5f, 0.5f);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			gluCylinder(pQuadric, 0.1f, 0.1f, 0.55f, 8, 8);
		}

		if (pCell->Type != CCell::ObjStrTube) {
			glTranslatef(0.0f, 0.0f, 0.5f);
			gluSphere(pQuadric, 0.14f, 5, 5);
		}
	}

	gluDeleteQuadric(pQuadric);
	return fRedisplay;
}


void COpenGL::DrawSender(CCell* /*pCell*/)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrSender]);

	GLfloat SenderVert[] = {
		-0.35f, 0.45f, 0.3f,	//0
		 0.35f, 0.45f, 0.3f,	//1
		 0.35f,-0.45f, 0.3f,	//2
		-0.35f,-0.45f, 0.3f,	//3
		 0.35f, 0.45f,-0.1f,	//4
		-0.35f, 0.45f,-0.1f,	//5
		 0.35f,-0.45f,-0.1f,	//6
		-0.35f,-0.45f,-0.1f		//7
	};
	GLushort SenderInd[] = {
		0, 1, 2, 3,				//Front
		0, 1, 4, 5,				//Top
		3, 2, 6, 7,				//Bottom
		0, 5, 7, 3,				//Left
		1, 4, 6, 2				//Right
	};
	GLshort SenderText[] = {
		0, 1, 1, 1, 1, 0, 0, 0,	//Front
		0, 1, 1, 1, 1, 0, 0, 0,	//Top
		0, 1, 1, 1, 1, 0, 0, 0,	//Bottom
		0, 1, 1, 1, 1, 0, 0, 0,	//Left
		0, 1, 1, 1, 1, 0, 0, 0	//Right
	};
	GLshort SenderNorm[] = {
		 0, 0,-1,				//Front
		 0, 1, 0,				//Top
		 0,-1, 0,				//Bottom
		-1, 0, 0,				//Left
		 1, 0, 0				//Right
	};

	glVertexPointer(3, GL_FLOAT, 0, SenderVert);
	glTexCoordPointer(2, GL_SHORT, 0, SenderText);
	glNormalPointer(GL_SHORT, 0, SenderNorm);
	glDrawElements(GL_QUADS, sizeof(SenderInd) / sizeof(SenderInd[0]), GL_UNSIGNED_SHORT, SenderInd);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void COpenGL::DrawReciever(CCell* pCell)
{
	//Draw system block
	glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrRcvBack]);
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(-0.4f,-0.17f, 0.2f);
		glTexCoord2i(1, 1); glVertex3f( 0.4f,-0.17f, 0.2f);
		glTexCoord2i(1, 0); glVertex3f( 0.4f,-0.4f, 0.2f);
		glTexCoord2i(0, 0); glVertex3f(-0.4f,-0.4f, 0.2f);
	glEnd();

	//Draw monitors prop
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2i(0, 1); glVertex3f(-0.1f, 0.0f, 0.15f);
		glTexCoord2i(1, 1); glVertex3f( 0.1f, 0.0f, 0.15f);
		glTexCoord2i(1, 0); glVertex3f( 0.1f,-0.2f, 0.15f);
		glTexCoord2i(0, 0); glVertex3f(-0.1f,-0.2f, 0.15f);
	glEnd();

	if (m_objGame.IsGameOver()) {
		//Make user happy!
		static GLfloat dAngle = 1.0f;
		static GLfloat dDir = 0.05f;
		if (dAngle > 5.0f || dAngle < -5.0f)
			dDir = -dDir;
		dAngle += dDir;
		glRotatef(dAngle, 0.0f, 0.0f, 1.0f);
	}

	//Draw monitor
	glBindTexture(GL_TEXTURE_2D, pCell->State ? m_aTextures[TxrRcvActive] : m_aTextures[TxrRcvPassive]);
	glBegin(GL_QUADS);
		glNormal3b(0, 0, 1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.4f, 0.4f, 0.2f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.4f, 0.4f, 0.2f);
		glTexCoord2f(1.0f, -0.75f); glVertex3f( 0.4f,-0.15f, 0.2f);
		glTexCoord2f(0.0f, -0.75f); glVertex3f(-0.4f,-0.15f, 0.2f);
	glEnd();
}
