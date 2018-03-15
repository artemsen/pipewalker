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

#pragma once

#include "winmgr.h"


#ifdef PW_USE_SDL

class CWinManagerSDL : public CWinManager
{
public:
	//! Default constructor
	CWinManagerSDL(CEventHandler& eventHandler) : CWinManager(eventHandler), m_ExitProgram(false) {}

public:
	//From CWinManager
	void CreateGLWindow(const int width, const int height);
	void PostRedisplay();
	void MainLoop();
	void OnApplicationExit();
	void ShowError(const char* err);
	void PostExit()		{ m_ExitProgram = true; }


protected:
	//From CWinManager
	void SwapBuffers() const;

private:
	bool m_ExitProgram;		///< Program exit flag
};

#endif	//PW_USE_SDL
