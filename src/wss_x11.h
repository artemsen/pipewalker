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

#ifdef PW_SYSTEM_NIX

#pragma once

#include "base.h"
#include "common.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
#include <GL/glx.h>


class CXServer : public CWinSubsystem
{
public:
	//! Default constructor
	CXServer(CEventHandler* pEventHandler) : CWinSubsystem(pEventHandler), m_xDisp(NULL), m_xCtx(NULL), m_fDone(false)	{}

	//! Default destructor
	~CXServer()	{}

public:	//From CWinSubsystem
	bool Initialize(void);
	int DoMainLoop(void);
	void PostExit(void);
	void PostRedisplay(void);
	void ShowErrorMessage(const char* pszErrorMsg);

private:
	Display*	m_xDisp;	///< X display
	Window		m_xWnd;		///< X window
	GLXContext	m_xCtx;		///< X drawing context
	bool		m_fDone;	///< done flag (used for stopping drawing and destroy window)
};

#endif	//PW_SYSTEM_NIX
