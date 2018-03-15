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

#pragma once

#include "base.h"
#include "common.h"
#include <GL/glut.h>


class CGlut : public CWinSubsystem
{
public:
	//! Default constructor
	CGlut(CEventHandler* pEventHandler) : CWinSubsystem(pEventHandler), m_nWnd(0)	{}

	//! Default destructor
	~CGlut()	{}

public:	//From CWinSubsystem
	bool Initialize(void);
	int DoMainLoop(void);
	void PostExit(void);
	void PostRedisplay(void);
	void ShowErrorMessage(const char* pszErrorMsg);

private:
	int				m_nWnd;		///< Number of our GLUT window
	static CGlut*	m_pThis;	///< This class

	//Glut callback functions
	static void OnMouseButton(int nButton, int nState, int nX, int nY);
	static void OnKeyPressed(unsigned char ucKey, int nX, int nY);
	static void OnDrawGLScene(void);
};

#endif	//PW_USE_GLUT
