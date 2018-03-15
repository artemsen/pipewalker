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

#ifdef PW_USE_GLUT

#include "wss_glut.h"


CGlut* CGlut::m_pThis = NULL;

bool CGlut::Initialize(void)
{
	assert(m_pThis == NULL);	//Already initialized?
	m_pThis = this;

	//GLUT initialization
	int argc = 1;	//dummy
	const char* argv = "NULL";
	glutInit(&argc, const_cast<char**>(&argv));
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
	glutInitWindowSize(PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT);  
	glutInitWindowPosition(200, 200);
	m_pThis->m_nWnd = glutCreateWindow(PW_WINDOW_TITLE);

	//Register callback functions
	glutDisplayFunc(&CGlut::OnDrawGLScene);
	glutReshapeFunc(&CGlut::OnWndResize);
	glutKeyboardFunc(&CGlut::OnKeyPressed);
	glutMouseFunc(&CGlut::OnMouseButton);
	return true;
}


int CGlut::DoMainLoop(void)
{
	glutMainLoop();
	return 0;
}


void CGlut::OnDrawGLScene(void)
{
	assert(m_pThis);					//Not initialized?
	m_pThis->m_pEventHandler->OnDraw();
	glutSwapBuffers();
}


void CGlut::OnMouseButton(int nButton, int nState, int nX, int nY)
{
	assert(m_pThis);					//Not initialized?
	if (nState != GLUT_DOWN)
		return;
	if (nButton == GLUT_LEFT_BUTTON || nButton == GLUT_MIDDLE_BUTTON || nButton == GLUT_RIGHT_BUTTON) {
		m_pThis->m_pEventHandler->OnMouseClick(
			nButton == GLUT_LEFT_BUTTON ? CEventHandler::LeftButton :
			nButton == GLUT_MIDDLE_BUTTON ? CEventHandler::MiddleButton : CEventHandler::RightButton,
			nX, nY);
	}
}


void CGlut::OnKeyPressed(unsigned char ucKey, int /* nX */, int /* nY */) 
{
	assert(m_pThis);					//Not initialized?
	m_pThis->m_pEventHandler->OnKeyPress(ucKey);
}


void CGlut::OnWndResize(int nWidth, int nHeight)
{
	assert(m_pThis);					//Not initialized?

	static const float dNeedAspect = static_cast<float>(PW_SCREEN_WIDTH) / static_cast<float>(PW_SCREEN_HEIGHT);
	const float dCurrAspect = static_cast<float>(nHeight) / static_cast<float>(nWidth);
	if (dNeedAspect != dCurrAspect) {
		nWidth = static_cast<long>(static_cast<float>(nHeight) * dNeedAspect);
		glutReshapeWindow(nWidth, nHeight);
	}
	m_pThis->m_pEventHandler->OnWndSizeChanged(nWidth, nHeight);
}


void CGlut::PostExit(void)
{
	assert(m_pThis);					//Not initialized?
	glutDestroyWindow(m_pThis->m_nWnd);	//Shut down our window
	exit(0);							//GLUT cannot return from main loop
}


void CGlut::PostRedisplay(void)
{
	glutPostRedisplay();
}


void CGlut::ShowErrorMessage(const char* pszErrorMsg)
{
#ifdef PW_SYSTEM_WINNT
	MessageBox(NULL, pszErrorMsg, "Error", MB_ICONERROR | MB_OK);
#else
	printf(pszErrorMsg);
	printf("\n");
#endif
}

#endif	//PW_USE_GLUT
