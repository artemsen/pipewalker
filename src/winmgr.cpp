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

#include "winmgr.h"

#define BASE_WIDTH		10.4f
#define ASPECT_RATIO	(static_cast<float>(PW_SCREEN_HEIGHT) / static_cast<float>(PW_SCREEN_WIDTH))

CWinManager::CWinManager(CEventHandler& eventHandler)
:	m_EventHandler(eventHandler),
	m_MouseWndX(0),
	m_MouseWndY(0),
	m_MouseWrldX(0.0f),
	m_MouseWrldY(0.0f)
{
}


void CWinManager::OnRenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	const GLdouble baseHalfSize = BASE_WIDTH / 2.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0.0 - baseHalfSize,	0.0 + baseHalfSize,
		0.0 - baseHalfSize * ASPECT_RATIO, 0.0 + baseHalfSize * ASPECT_RATIO,
		-1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m_EventHandler.RenderScene(m_MouseWrldX, m_MouseWrldY);
	SwapBuffers();
}


void CWinManager::OnKeyboardKeyDown(const char key)
{
	m_EventHandler.OnKeyboardKeyDown(key);
}


void CWinManager::OnMouseButtonDown(const int x, const int y, const MouseButton btn)
{
	UpdateMousePosition(x, y);
	m_EventHandler.OnMouseButtonDown(m_MouseWrldX, m_MouseWrldY, btn);
}


void CWinManager::OnMouseMove(const int x, const int y)
{
	UpdateMousePosition(x, y);
	m_EventHandler.OnMouseMove(m_MouseWrldX, m_MouseWrldY);
}


void CWinManager::InitializeOpenGL(const int width, const int height) const
{
	//Initialize OpenGL subsystem
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//We will use only arrays to draw
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glViewport(0, 0, width, height);
}


void CWinManager::UpdateMousePosition(const int x, const int y)
{
	if (x != m_MouseWndX || y != m_MouseWndY) {

		GLint glViewport[4];
		glGetIntegerv(GL_VIEWPORT, glViewport);
		if (x >= 0 && x <= glViewport[2] && y >= 0 && y <= glViewport[3]) {
			m_MouseWndX = x;
			m_MouseWndY = y;
			m_MouseWrldX = static_cast<float>(m_MouseWndX) * (BASE_WIDTH / static_cast<float>(glViewport[2])) - BASE_WIDTH / 2.0f;
			m_MouseWrldY = ASPECT_RATIO * (BASE_WIDTH / 2.0f - static_cast<float>(m_MouseWndY) * (BASE_WIDTH / static_cast<float>(glViewport[3])));
		}
	}
}
