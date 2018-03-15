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

#pragma once

#include "winmgr.h"


#ifdef PW_USE_WINCE

class CWinManagerWinCE : public CWinManager
{
public:
	//! Constructor
	CWinManagerWinCE(CEventHandler& eventHandler);

public:
	//From CWinManager
	void CreateGLWindow(const int width, const int height);
	void PostRedisplay();
	void MainLoop();
	void OnApplicationExit();
	void ShowError(const char* err);
	void PostExit();

protected:
	//From CWinManager
	void SwapBuffers() const;

private:
	/**
	 * Destroy window and post exit message
	 */
	void DestroyAndQuit();

	/**
	 * The WindowProc function (for more information see MSDN)
	 * \param wnd Handle to the window
	 * \param msg Specifies the message
	 * \param wParam Specifies additional message information
	 * \param lParam Specifies additional message information
	 * \return result of the message processing, depends on the message sent
	 */
	static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	EGLDisplay	_Display;
	EGLConfig	_Config;
	EGLContext	_Context;
	EGLSurface	_WindowSurface;

	HWND	_Wnd;			///< Holds Our Window Handle
	bool	_Redisplay;		///< Redisplay flag
};

#endif	//PW_USE_WINCE
